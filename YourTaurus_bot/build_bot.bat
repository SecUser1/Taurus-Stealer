set GOOS=windows
set GOARCH=386
go build -o bot.exe -ldflags="-w -s" -trimpath main.go