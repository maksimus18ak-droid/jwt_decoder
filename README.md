🔑 JWT Decoder — декодирование и подсветка JSON
Версия: 1.0.0 | Лицензия: MIT | Статус: ✅ Активная разработка

https://img.shields.io/github/repo-size/yourusername/jwt-decoder https://img.shields.io/github/last-commit/yourusername/jwt-decoder https://img.shields.io/github/languages/count/yourusername/jwt-decoder

🧩 Описание
JWT Decoder — это консольная утилита для декодирования JSON Web Tokens (JWT) с подсветкой синтаксиса JSON. Программа принимает JWT-токен (или читает из файла), разделяет его на три части (Header, Payload, Signature), декодирует Base64Url-данные и отображает содержимое в удобочитаемом виде с цветовой подсветкой:

Header — синий цвет

Payload — зелёный цвет

Signature — серый цвет (не декодируется, так как это бинарные данные)

Дополнительные возможности:

✅ Валидация структуры JWT (три сегмента, разделённые точками)

✅ Декодирование Base64Url с автоматическим исправлением padding

✅ Подсветка ключей и строк в JSON

✅ Вывод в компактном или развёрнутом виде (pretty print)

✅ Опция проверки подписи (HMAC, RSA) — опционально

✅ Кроссплатформенность (Linux, macOS, Windows)

Проект содержит 8 полноценных реализаций на разных языках программирования. Все версии используют встроенные средства для работы с JSON и Base64, а для цветного вывода — ANSI-коды или нативные API.

✨ Возможности
Функция	Описание
Декодирование JWT	Разделение токена на сегменты, декодирование Base64Url
Подсветка JSON	Цветная визуализация ключей, строк, чисел, булевых значений
Pretty Print	Форматирование с отступами для удобного чтения
Проверка подписи	Верификация HMAC (опционально)
Чтение из файла	Поддержка передачи токена через аргумент или из файла
Кроссплатформенность	Работает везде, где есть соответствующий интерпретатор/компилятор
📦 Установка и запуск
Каждая реализация находится в отдельной папке. Для запуска требуется соответствующий компилятор/интерпретатор и библиотеки (где необходимо).

Язык	Файл	Команда запуска
Python	jwt_decoder.py	python3 jwt_decoder.py <token>
Go	jwt_decoder.go	go run jwt_decoder.go <token>
Rust	jwt_decoder.rs	cargo run -- <token>
C++	jwt_decoder.cpp	g++ -std=c++17 -o jwt_decoder jwt_decoder.cpp && ./jwt_decoder <token>
Java	JwtDecoder.java	javac JwtDecoder.java && java JwtDecoder <token>
C#	jwt_decoder.cs	dotnet run <token>
Ruby	jwt_decoder.rb	ruby jwt_decoder.rb <token>
Node.js	jwt_decoder.js	node jwt_decoder.js <token>
📂 Структура репозитория
text
.
├── README.md
├── python/
│   └── jwt_decoder.py
├── go/
│   └── jwt_decoder.go
├── rust/
│   ├── Cargo.toml
│   └── src/
│       └── main.rs
├── cpp/
│   └── jwt_decoder.cpp
├── java/
│   └── JwtDecoder.java
├── csharp/
│   └── jwt_decoder.cs
├── ruby/
│   └── jwt_decoder.rb
└── javascript/
    ├── package.json
    └── jwt_decoder.js
🎮 Использование
bash
# Декодировать токен, переданный как аргумент
jwt_decoder eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJzdWIiOiIxMjM0NTY3ODkwIiwibmFtZSI6IkpvaG4gRG9lIiwiaWF0IjoxNTE2MjM5MDIyfQ.SflKxwRJSMeKKF2QT4fwpMeJf36POk6yJV_adQssw5c

# Чтение из файла (если токен содержит пробелы или переносы)
jwt_decoder --file token.txt

# Вывод в компактном виде (без отступов)
jwt_decoder <token> --compact

# Проверка HMAC подписи (ключ указывается через --secret)
jwt_decoder <token> --secret mysecret
🛠️ Особенности реализаций
Python – использует json, base64, argparse; цвет через ANSI.

Go – encoding/json, encoding/base64, flag; ANSI-цвета.

Rust – serde_json, base64, clap; termcolor для подсветки.

C++ – nlohmann/json, base64 (самописный или из Boost); ANSI.

Java – java.util.Base64, Jackson или Gson для JSON.

C# – System.Text.Json, Convert.FromBase64String; Console.ForegroundColor.

Ruby – json, base64; встроенный optparse.

Node.js – json, Buffer для Base64; chalk для цвета.

🤝 Вклад
PR и issues приветствуются. Добавляйте поддержку асимметричных подписей, улучшайте цветовую схему, расширяйте функциональность.

📄 Лицензия
MIT License.

