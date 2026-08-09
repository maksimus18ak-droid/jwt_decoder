// jwt_decoder.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Text;
using System.Text.Json;

class JwtDecoder
{
    private const string RESET = "\x1b[0m";
    private const string KEY = "\x1b[94m";
    private const string STR = "\x1b[92m";
    private const string NUM = "\x1b[93m";
    private const string BOOL = "\x1b[95m";
    private const string NULL = "\x1b[90m";
    private const string HEADER = "\x1b[96m";
    private const string SIG = "\x1b[90m";

    private static string Base64UrlDecode(string data)
    {
        // Добавляем padding
        int pad = 4 - (data.Length % 4);
        if (pad != 4) data += new string('=', pad);
        data = data.Replace('-', '+').Replace('_', '/');
        byte[] bytes = Convert.FromBase64String(data);
        return Encoding.UTF8.GetString(bytes);
    }

    private static string ColorizeJson(JsonElement elem, int indent, int level)
    {
        string spaces = new string(' ', level * indent);
        switch (elem.ValueKind)
        {
            case JsonValueKind.Object:
                if (elem.GetRawText() == "{}") return "{}";
                var sb = new StringBuilder();
                sb.AppendLine("{");
                int i = 0;
                foreach (var prop in elem.EnumerateObject())
                {
                    string key = prop.Name;
                    string valStr = ColorizeJson(prop.Value, indent, level + 1);
                    sb.Append(spaces).Append(' ', indent)
                      .Append(KEY).Append('"').Append(key).Append('"').Append(RESET)
                      .Append(": ").Append(valStr);
                    if (i < elem.EnumerateObject().Count() - 1) sb.Append(',');
                    sb.AppendLine();
                    i++;
                }
                sb.Append(spaces).Append('}');
                return sb.ToString();
            case JsonValueKind.Array:
                if (elem.GetArrayLength() == 0) return "[]";
                sb = new StringBuilder();
                sb.AppendLine("[");
                int j = 0;
                foreach (var item in elem.EnumerateArray())
                {
                    string valStr = ColorizeJson(item, indent, level + 1);
                    sb.Append(spaces).Append(' ', indent).Append(valStr);
                    if (j < elem.GetArrayLength() - 1) sb.Append(',');
                    sb.AppendLine();
                    j++;
                }
                sb.Append(spaces).Append(']');
                return sb.ToString();
            case JsonValueKind.String:
                return $"{STR}\"{elem.GetString()}\"{RESET}";
            case JsonValueKind.True:
            case JsonValueKind.False:
                return $"{BOOL}{elem.GetBoolean().ToString().ToLower()}{RESET}";
            case JsonValueKind.Null:
                return $"{NULL}null{RESET}";
            case JsonValueKind.Number:
                return $"{NUM}{elem.GetRawText()}{RESET}";
            default:
                return elem.GetRawText();
        }
    }

    private static void DecodeJwt(string token)
    {
        string[] parts = token.Split('.');
        if (parts.Length != 3) throw new Exception("Неверный JWT: ожидается 3 сегмента");
        string headerB64 = parts[0], payloadB64 = parts[1];
        string sig = parts[2];
        string headerStr = Base64UrlDecode(headerB64);
        string payloadStr = Base64UrlDecode(payloadB64);
        using JsonDocument docHeader = JsonDocument.Parse(headerStr);
        using JsonDocument docPayload = JsonDocument.Parse(payloadStr);
        int indent = 2;
        Console.WriteLine($"{HEADER}--- Header ---{RESET}");
        Console.WriteLine(ColorizeJson(docHeader.RootElement, indent, 0));
        Console.WriteLine($"{HEADER}--- Payload ---{RESET}");
        Console.WriteLine(ColorizeJson(docPayload.RootElement, indent, 0));
        Console.WriteLine($"{SIG}--- Signature ---{RESET}");
        Console.WriteLine($"{SIG}{sig}{RESET}");
    }

    static void Main(string[] args)
    {
        string token = null;
        string filePath = null;
        bool compact = false;
        for (int i=0; i<args.Length; i++)
        {
            if (args[i] == "--file" && i+1 < args.Length)
                filePath = args[++i];
            else if (args[i] == "--compact")
                compact = true;
            else
                token = args[i];
        }
        if (filePath != null)
            token = File.ReadAllText(filePath).Trim();
        if (string.IsNullOrEmpty(token))
        {
            Console.Error.WriteLine("Не указан JWT-токен.");
            Environment.Exit(1);
        }
        DecodeJwt(token);
    }
}
