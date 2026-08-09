# jwt_decoder.py
import sys
import base64
import json
import argparse
import re

# ANSI-цвета
COLOR_RESET = "\033[0m"
COLOR_KEY = "\033[94m"      # синий
COLOR_STRING = "\033[92m"   # зелёный
COLOR_NUMBER = "\033[93m"   # жёлтый
COLOR_BOOL = "\033[95m"     # пурпурный
COLOR_NULL = "\033[90m"     # серый
COLOR_HEADER = "\033[96m"   # голубой
COLOR_SIGNATURE = "\033[90m" # тёмно-серый

def base64url_decode(data):
    """Декодирует Base64Url в строку."""
    # Добавляем padding, если необходимо
    padding = 4 - (len(data) % 4)
    if padding != 4:
        data += '=' * padding
    return base64.urlsafe_b64decode(data).decode('utf-8')

def colorize_json(obj, indent=2, level=0):
    """Рекурсивно форматирует JSON с подсветкой."""
    if isinstance(obj, dict):
        lines = []
        if level == 0:
            lines.append("{")
        else:
            lines.append("{")
        for i, (k, v) in enumerate(obj.items()):
            key_str = f"{COLOR_KEY}\"{k}\"{COLOR_RESET}: "
            if isinstance(v, dict):
                val_str = colorize_json(v, indent, level+1)
            elif isinstance(v, list):
                val_str = colorize_json(v, indent, level+1)
            elif isinstance(v, str):
                val_str = f"{COLOR_STRING}\"{v}\"{COLOR_RESET}"
            elif isinstance(v, bool):
                val_str = f"{COLOR_BOOL}{str(v).lower()}{COLOR_RESET}"
            elif v is None:
                val_str = f"{COLOR_NULL}null{COLOR_RESET}"
            elif isinstance(v, (int, float)):
                val_str = f"{COLOR_NUMBER}{v}{COLOR_RESET}"
            else:
                val_str = str(v)
            lines.append(" " * (level+1) * indent + key_str + val_str)
            if i < len(obj)-1:
                lines[-1] += ","
        if level == 0:
            lines.append("}")
        else:
            lines.append(" " * level * indent + "}")
        return "\n".join(lines)
    elif isinstance(obj, list):
        lines = ["["]
        for item in obj:
            if isinstance(item, dict):
                lines.append(colorize_json(item, indent, level+1))
            else:
                lines.append(" " * (level+1) * indent + colorize_json(item, indent, level+1))
        lines.append(" " * level * indent + "]")
        return "\n".join(lines)
    else:
        if isinstance(obj, str):
            return f"{COLOR_STRING}\"{obj}\"{COLOR_RESET}"
        elif isinstance(obj, bool):
            return f"{COLOR_BOOL}{str(obj).lower()}{COLOR_RESET}"
        elif obj is None:
            return f"{COLOR_NULL}null{COLOR_RESET}"
        elif isinstance(obj, (int, float)):
            return f"{COLOR_NUMBER}{obj}{COLOR_RESET}"
        return str(obj)

def decode_jwt(token, pretty=True):
    """Декодирует JWT и возвращает header, payload, signature."""
    parts = token.split('.')
    if len(parts) != 3:
        raise ValueError("Неверный JWT: должно быть 3 сегмента")
    header_b64, payload_b64, signature_b64 = parts
    try:
        header_json = base64url_decode(header_b64)
        payload_json = base64url_decode(payload_b64)
        header = json.loads(header_json)
        payload = json.loads(payload_json)
    except Exception as e:
        raise ValueError(f"Ошибка декодирования Base64: {e}")
    return header, payload, signature_b64

def main():
    parser = argparse.ArgumentParser(description="JWT Decoder с подсветкой JSON")
    parser.add_argument("token", nargs="?", help="JWT-токен")
    parser.add_argument("--file", "-f", help="Читать токен из файла")
    parser.add_argument("--compact", action="store_true", help="Вывод без отступов")
    args = parser.parse_args()

    token = args.token
    if args.file:
        with open(args.file, 'r') as f:
            token = f.read().strip()
    if not token:
        print("Не указан JWT-токен.", file=sys.stderr)
        sys.exit(1)

    try:
        header, payload, signature = decode_jwt(token)
    except Exception as e:
        print(f"Ошибка: {e}", file=sys.stderr)
        sys.exit(1)

    indent = None if args.compact else 2
    print(f"{COLOR_HEADER}--- Header ---{COLOR_RESET}")
    print(colorize_json(header, indent=indent if indent else 0))
    print(f"{COLOR_HEADER}--- Payload ---{COLOR_RESET}")
    print(colorize_json(payload, indent=indent if indent else 0))
    print(f"{COLOR_SIGNATURE}--- Signature ---{COLOR_RESET}")
    print(f"{COLOR_SIGNATURE}{signature}{COLOR_RESET}")

if __name__ == "__main__":
    main()
