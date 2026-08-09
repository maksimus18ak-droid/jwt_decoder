// JwtDecoder.java
import java.io.*;
import java.nio.file.*;
import java.util.*;
import java.util.Base64;
import com.google.gson.*;

public class JwtDecoder {
    private static final String RESET = "\033[0m";
    private static final String KEY = "\033[94m";
    private static final String STR = "\033[92m";
    private static final String NUM = "\033[93m";
    private static final String BOOL = "\033[95m";
    private static final String NULL = "\033[90m";
    private static final String HEADER = "\033[96m";
    private static final String SIG = "\033[90m";

    private static String base64UrlDecode(String data) {
        // Добавляем padding
        int pad = 4 - (data.length() % 4);
        if (pad != 4) data += "=".repeat(pad);
        // Заменяем '-' -> '+', '_' -> '/'
        data = data.replace('-', '+').replace('_', '/');
        return new String(Base64.getDecoder().decode(data));
    }

    private static String colorizeJson(JsonElement elem, int indent, int level) {
        String spaces = " ".repeat(level * indent);
        if (elem.isJsonObject()) {
            JsonObject obj = elem.getAsJsonObject();
            if (obj.entrySet().isEmpty()) return "{}";
            StringBuilder sb = new StringBuilder();
            sb.append("{\n");
            int i = 0;
            for (Map.Entry<String, JsonElement> entry : obj.entrySet()) {
                String key = entry.getKey();
                JsonElement val = entry.getValue();
                String valStr = colorizeJson(val, indent, level+1);
                sb.append(spaces).append(" ".repeat(indent))
                  .append(KEY).append("\"").append(key).append("\"").append(RESET).append(": ")
                  .append(valStr);
                if (i < obj.entrySet().size() - 1) sb.append(",");
                sb.append("\n");
                i++;
            }
            sb.append(spaces).append("}");
            return sb.toString();
        } else if (elem.isJsonArray()) {
            JsonArray arr = elem.getAsJsonArray();
            if (arr.size() == 0) return "[]";
            StringBuilder sb = new StringBuilder();
            sb.append("[\n");
            for (int i=0; i<arr.size(); i++) {
                String valStr = colorizeJson(arr.get(i), indent, level+1);
                sb.append(spaces).append(" ".repeat(indent)).append(valStr);
                if (i < arr.size() - 1) sb.append(",");
                sb.append("\n");
            }
            sb.append(spaces).append("]");
            return sb.toString();
        } else if (elem.isJsonPrimitive()) {
            JsonPrimitive prim = elem.getAsJsonPrimitive();
            if (prim.isString()) {
                return STR + "\"" + prim.getAsString() + "\"" + RESET;
            } else if (prim.isBoolean()) {
                return BOOL + prim.getAsBoolean() + RESET;
            } else if (prim.isNumber()) {
                return NUM + prim.getAsNumber() + RESET;
            } else {
                return prim.toString();
            }
        } else if (elem.isJsonNull()) {
            return NULL + "null" + RESET;
        }
        return elem.toString();
    }

    private static void decodeJwt(String token, Gson gson) throws Exception {
        String[] parts = token.split("\\.");
        if (parts.length != 3) throw new Exception("Неверный JWT: ожидается 3 сегмента");
        String headerB64 = parts[0], payloadB64 = parts[1];
        String sig = parts[2];
        String headerStr = base64UrlDecode(headerB64);
        String payloadStr = base64UrlDecode(payloadB64);
        JsonObject header = gson.fromJson(headerStr, JsonObject.class);
        JsonObject payload = gson.fromJson(payloadStr, JsonObject.class);
        int indent = 2;
        System.out.println(HEADER + "--- Header ---" + RESET);
        System.out.println(colorizeJson(header, indent, 0));
        System.out.println(HEADER + "--- Payload ---" + RESET);
        System.out.println(colorizeJson(payload, indent, 0));
        System.out.println(SIG + "--- Signature ---" + RESET);
        System.out.println(SIG + sig + RESET);
    }

    public static void main(String[] args) throws Exception {
        String token = null;
        String filePath = null;
        boolean compact = false;
        for (int i=0; i<args.length; i++) {
            if (args[i].equals("--file") && i+1 < args.length) {
                filePath = args[++i];
            } else if (args[i].equals("--compact")) {
                compact = true;
            } else {
                token = args[i];
            }
        }
        if (filePath != null) {
            token = new String(Files.readAllBytes(Paths.get(filePath))).trim();
        }
        if (token == null || token.isEmpty()) {
            System.err.println("Не указан JWT-токен.");
            System.exit(1);
        }
        Gson gson = new GsonBuilder().create();
        decodeJwt(token, gson);
    }
}
