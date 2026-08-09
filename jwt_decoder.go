// jwt_decoder.go
package main

import (
	"encoding/base64"
	"encoding/json"
	"flag"
	"fmt"
	"os"
	"strings"
)

const (
	reset  = "\033[0m"
	keyClr = "\033[94m"
	strClr = "\033[92m"
	numClr = "\033[93m"
	boolClr = "\033[95m"
	nullClr = "\033[90m"
	headerClr = "\033[96m"
	sigClr = "\033[90m"
)

func base64UrlDecode(data string) (string, error) {
	// Добавляем padding
	if l := len(data) % 4; l > 0 {
		data += strings.Repeat("=", 4-l)
	}
	b, err := base64.URLEncoding.DecodeString(data)
	if err != nil {
		return "", err
	}
	return string(b), nil
}

func colorizeValue(v interface{}, indent int, level int) string {
	spaces := strings.Repeat(" ", level*indent)
	switch val := v.(type) {
	case map[string]interface{}:
		if len(val) == 0 {
			return "{}"
		}
		lines := []string{"{"}
		i := 0
		for k, v2 := range val {
			keyStr := fmt.Sprintf("%s\"%s\"%s: ", keyClr, k, reset)
			valStr := colorizeValue(v2, indent, level+1)
			line := spaces + strings.Repeat(" ", indent) + keyStr + valStr
			if i < len(val)-1 {
				line += ","
			}
			lines = append(lines, line)
			i++
		}
		lines = append(lines, spaces+"}")
		return strings.Join(lines, "\n")
	case []interface{}:
		if len(val) == 0 {
			return "[]"
		}
		lines := []string{"["}
		for i, v2 := range val {
			line := spaces + strings.Repeat(" ", indent) + colorizeValue(v2, indent, level+1)
			if i < len(val)-1 {
				line += ","
			}
			lines = append(lines, line)
		}
		lines = append(lines, spaces+"]")
		return strings.Join(lines, "\n")
	case string:
		return fmt.Sprintf("%s\"%s\"%s", strClr, val, reset)
	case bool:
		return fmt.Sprintf("%s%t%s", boolClr, val, reset)
	case nil:
		return fmt.Sprintf("%snull%s", nullClr, reset)
	case float64, int64, int:
		return fmt.Sprintf("%s%v%s", numClr, val, reset)
	default:
		return fmt.Sprintf("%v", val)
	}
}

func colorizeJSON(data interface{}, indent int) string {
	return colorizeValue(data, indent, 0)
}

func decodeJWT(token string) (map[string]interface{}, map[string]interface{}, string, error) {
	parts := strings.Split(token, ".")
	if len(parts) != 3 {
		return nil, nil, "", fmt.Errorf("неверный JWT: ожидается 3 сегмента")
	}
	headerB64, payloadB64, sigB64 := parts[0], parts[1], parts[2]
	headerStr, err1 := base64UrlDecode(headerB64)
	if err1 != nil {
		return nil, nil, "", fmt.Errorf("ошибка декодирования header: %v", err1)
	}
	payloadStr, err2 := base64UrlDecode(payloadB64)
	if err2 != nil {
		return nil, nil, "", fmt.Errorf("ошибка декодирования payload: %v", err2)
	}
	var header, payload map[string]interface{}
	if err := json.Unmarshal([]byte(headerStr), &header); err != nil {
		return nil, nil, "", fmt.Errorf("ошибка парсинга header JSON: %v", err)
	}
	if err := json.Unmarshal([]byte(payloadStr), &payload); err != nil {
		return nil, nil, "", fmt.Errorf("ошибка парсинга payload JSON: %v", err)
	}
	return header, payload, sigB64, nil
}

func main() {
	var fileFlag string
	var compact bool
	flag.StringVar(&fileFlag, "file", "", "Читать токен из файла")
	flag.BoolVar(&compact, "compact", false, "Вывод без отступов")
	flag.Parse()
	var token string
	if fileFlag != "" {
		data, err := os.ReadFile(fileFlag)
		if err != nil {
			fmt.Fprintf(os.Stderr, "Ошибка чтения файла: %v\n", err)
			os.Exit(1)
		}
		token = strings.TrimSpace(string(data))
	} else {
		args := flag.Args()
		if len(args) < 1 {
			fmt.Fprintf(os.Stderr, "Не указан JWT-токен.\n")
			os.Exit(1)
		}
		token = args[0]
	}
	header, payload, sig, err := decodeJWT(token)
	if err != nil {
		fmt.Fprintf(os.Stderr, "Ошибка: %v\n", err)
		os.Exit(1)
	}
	indent := 2
	if compact {
		indent = 0
	}
	fmt.Printf("%s--- Header ---%s\n", headerClr, reset)
	fmt.Println(colorizeJSON(header, indent))
	fmt.Printf("%s--- Payload ---%s\n", headerClr, reset)
	fmt.Println(colorizeJSON(payload, indent))
	fmt.Printf("%s--- Signature ---%s\n", sigClr, reset)
	fmt.Printf("%s%s%s\n", sigClr, sig, reset)
}
