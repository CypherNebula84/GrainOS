struct File {
  String name;
  String content;
  bool used;
};

const int MAX_FILES = 5;
File files[MAX_FILES];

void clearScreen() {
  for (int i = 0; i < 50; i++) {
    Serial.println();
  }
}

void listFiles() {
  Serial.println("Files:");
  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].used) {
      Serial.println("- " + files[i].name);
    }
  }
}

bool isValidFileName(String name) {
  for (int i = 0; i < name.length(); i++) {
    char c = name[i];
    if (!isalnum(c) && c != '_') {
      return false;
    }
  }
  return true;
}

void printError(String message) {
  Serial.println("Error: " + message);
}

void writeFile(String name, String content) {
  if (!isValidFileName(name)) {
    printError("Invalid file name. Use letters, numbers, and underscores.");
    return;
  }
  for (int i = 0; i < MAX_FILES; i++) {
    if (!files[i].used) {
      files[i].name = name;
      files[i].content = content;
      files[i].used = true;
      Serial.println("File written: " + name);
      return;
    }
  }
  printError("No space left in RAM! Maximum files: " + String(MAX_FILES));
}

void readFile(String name) {
  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].used && files[i].name == name) {
      Serial.println("Content of " + name + ": " + files[i].content);
      return;
    }
  }
  printError("File not found!");
}

void deleteFile(String name) {
  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].used && files[i].name == name) {
      files[i].used = false;
      Serial.println("Deleted file: " + name);
      return;
    }
  }
  printError("File not found!");
}

void showHelp() {
  Serial.println("Available commands:");
  Serial.println("  clear - Clears the screen");
  Serial.println("  echo <message> - Prints the message");
  Serial.println("  format(\"text,text,text\") - Formats input");
  Serial.println("  write <filename> <content> - Writes content to a file");
  Serial.println("  read <filename> - Reads and prints the content of a file");
  Serial.println("  delete <filename> - Deletes a file");
  Serial.println("  ls - Lists all files");
  Serial.println("  help - Displays this help message");
  Serial.println("  calc <number> <operator> <number> - calculates command");
  Serial.println("  run <script.gs> - Runs a script file");
  Serial.println("  loop <number> <command> - Loops a command the inputted amout of times");
  Serial.println("  high <pin-number> - sets the inputted pin to high");
  Serial.println("  low <pin-number> - sets the inputted pin to low");
}

void calculate(String input) {
  int firstSpace = input.indexOf(' ');
  int secondSpace = input.indexOf(' ', firstSpace + 1);

  if (firstSpace == -1 || secondSpace == -1) {
    printError("Usage: calc <number> <operator> <number>");
    return;
  }

  String num1Str = input.substring(0, firstSpace);
  String op = input.substring(firstSpace + 1, secondSpace);
  String num2Str = input.substring(secondSpace + 1);

  float num1 = num1Str.toFloat();
  float num2 = num2Str.toFloat();
  float result;

  if (op == "+") {
    result = num1 + num2;
  } else if (op == "-") {
    result = num1 - num2;
  } else if (op == "*") {
    result = num1 * num2;
  } else if (op == "/") {
    if (num2 == 0) {
      printError("Division by zero");
      return;
    }
    result = num1 / num2;
  } else {
    printError("Unknown operator. Use +, -, *, or /");
    return;
  }

  Serial.println(String(result));
}

void runScript(String name) {
  if (!name.endsWith(".gs")) {
    printError("Must end with .gs!");
    return;
  }

  for (int i = 0; i < MAX_FILES; i++) {
    if (files[i].used && files[i].name == name) {
      Serial.println("Running script: " + name);
      String content = files[i].content;
      content.replace("\\n", "\n");

      int start = 0;
      while (start < content.length()) {
        int end = content.indexOf('\n', start);
        if (end == -1) end = content.length();

        String command = content.substring(start, end);
        command.trim();

        Serial.print("Extracted Command: [");
        Serial.print(command);
        Serial.println("]");

        if (command.length() > 0) {
          Serial.println("> " + command);
          processCommand(command);
        }

        start = end + 1;
      }
      return;
    }
  }
  printError("No script found.");
}

void loopCommand(String input) {
  int firstSpace = input.indexOf(' ');
  if (firstSpace == -1) {
    printError("Use `help`");
    return;
  }

  String numStr = input.substring(0, firstSpace);
  String command = input.substring(firstSpace + 1);

  int times = numStr.toInt();

  if (times <= 0) {
    printError("Must be positive number");
    return;
  }

  for (int i = 0; i < times; i++) {
    processCommand(command);
  }
}

void setPinLow(String input) {
  int pin = input.toInt();
  if (pin >= 0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
    Serial.println("Pin " + String(pin) + " set to LOW");
  } else {
    printError("Invalid pin number");
  }
}

void setPinHigh(String input) {
  int pin = input.toInt();
  if (pin >= 0) {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, HIGH);
    Serial.println("Pin " + String(pin) + " set to HIGH");
  } else {
    printError("Invalid pin number");
  }
}

void processCommand(String input) {
  input.trim();

  if (input == "clear") {
    clearScreen();
  } else if (input.startsWith("echo ")) {
    String message = input.substring(5);
    message.trim();
    Serial.println(message);
  } else if (input.startsWith("format(") && input.endsWith(")")) {
    input = input.substring(7, input.length() - 1);
    input.replace("\"", "");
    input.replace(",", " ");
    Serial.println(input);
  } else if (input.startsWith("write ")) {
    int space = input.indexOf(' ', 6);
    if (space != -1) {
      String name = input.substring(6, space);
      String content = input.substring(space + 1);
      writeFile(name, content);
    } else {
      printError("use `help`");
    }
  } else if (input.startsWith("read ")) {
    String name = input.substring(5);
    readFile(name);
  } else if (input.startsWith("delete ")) {
    String name = input.substring(7);
    deleteFile(name);
  } else if (input == "ls") {
    listFiles();
  } else if (input == "help") {
    showHelp();
  } else if (input.startsWith("calc ")) {
    calculate(input.substring(5));
  } else if (input == "cat") {
    Serial.println(" /\\_/\\");
    Serial.println("( o.o )");
    Serial.println("/> ^ <\\");
  } else if (input.startsWith("loop ")) {
    loopCommand(input.substring(5));
  } else if (input.startsWith("high ")) {
    setPinHigh(input.substring(5));
  } else if (input.startsWith("low ")) {
    setPinLow(input.substring(4));
  } else {
    Serial.println("Unknown command " + input);
  }
}

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("Welcome to GrainOS 1.8.3, `help` for a list of commands");
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil(' \n');
    input.trim();

    if (input.startsWith("run ")) {
      String scriptName = input.substring(4);
      runScript(scriptName);
    } else {
      processCommand(input);
    }
  }
}
