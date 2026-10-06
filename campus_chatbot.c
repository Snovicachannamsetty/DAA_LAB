#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
 
#define TABLE_SIZE 101
#define MAX_FAQS 100
#define MAX_KEYWORDS 20
#define MAX_KEYWORD_LEN 40
#define MAX_INTENT_LEN 80
#define MAX_RESPONSE_LEN 700
#define MAX_LINE_LEN 1200
#define KB_FILE "helpdesk_kb.txt"
#define LOG_FILE "chat_log.txt"
 
typedef struct {
    char intent[MAX_INTENT_LEN];
    char keywords[MAX_KEYWORDS][MAX_KEYWORD_LEN];
    int keywordCount;
    char response[MAX_RESPONSE_LEN];
} FAQ;
 
typedef struct KeywordNode {
    char keyword[MAX_KEYWORD_LEN];
    int faqIndex;
    struct KeywordNode *next;
} KeywordNode;
 
FAQ faqList[MAX_FAQS];
int faqCount = 0;
KeywordNode *hashTable[TABLE_SIZE];
 
void clearScreen();
void pauseScreen();
void printHeader();
void printLine(char ch, int length);
void removeNewline(char str[]);
void trimSpaces(char str[]);
void toLowerCase(char str[]);
void normalizeQuery(const char input[], char output[]);
unsigned int hashFunction(const char *word);
void initializeHashTable();
void freeHashTable();
void insertKeyword(const char *keyword, int faqIndex);
KeywordNode *searchKeyword(const char *keyword);
void createDefaultKnowledgeBase();
int loadKnowledgeBase();
void buildHashIndex();
void displayAllCategories();
void chatbotMode();
void findBestAnswer(const char query[]);
void addNewFAQ();
void appendFAQToFile(FAQ faq);
void logChat(const char query[], const char intent[], const char response[]);
void showProjectInfo();
void showHelp();
int getMenuChoice();
 
int main() {
    int choice;
 
    initializeHashTable();
 
    if (!loadKnowledgeBase()) {
        printf("Knowledge base file not found. Creating default knowledge base...\n");
        createDefaultKnowledgeBase();
        loadKnowledgeBase();
    }
 
    buildHashIndex();
 
    while (1) {
        clearScreen();
        printHeader();
        printf("\n");
        printf("  1. Ask Campus Help Desk Chatbot\n");
        printf("  2. View All Help Desk Categories\n");
        printf("  3. Add New FAQ to Knowledge Base\n");
        printf("  4. Reload Knowledge Base\n");
        printf("  5. Project Information\n");
        printf("  6. Help / How to Ask Questions\n");
        printf("  7. Exit\n");
        printf("\n");
        printLine('-', 72);
 
        choice = getMenuChoice();
 
        switch (choice) {
            case 1:
                clearScreen();
                chatbotMode();
                break;
 
            case 2:
                clearScreen();
                displayAllCategories();
                pauseScreen();
                break;
 
            case 3:
                clearScreen();
                addNewFAQ();
                pauseScreen();
                break;
 
            case 4:
                freeHashTable();
                initializeHashTable();
                faqCount = 0;
                loadKnowledgeBase();
                buildHashIndex();
                printf("\nKnowledge base reloaded successfully. Total categories: %d\n", faqCount);
                pauseScreen();
                break;
 
            case 5:
                clearScreen();
                showProjectInfo();
                pauseScreen();
                break;
 
            case 6:
                clearScreen();
                showHelp();
                pauseScreen();
                break;
 
            case 7:
                freeHashTable();
                printf("\nThank you for using Campus Help Desk Chatbot.\n");
                printf("Chat log is stored in '%s'.\n\n", LOG_FILE);
                return 0;
        }
    }
}
 
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    printf("\033[2J\033[H");
#endif
}
 
void pauseScreen() {
    char temp[10];
    printf("\nPress ENTER to continue...");
    fgets(temp, sizeof(temp), stdin);
}
 
void printHeader() {
    printLine('=', 72);
    printf("                 CAMPUS HELP DESK CHATBOT IN C\n");
    printf("             Keyword-Based AI Mini Project using Hash Table\n");
    printLine('=', 72);
    printf(" Knowledge Base Categories Loaded: %d\n", faqCount);
    printLine('-', 72);
}
 
void printLine(char ch, int length) {
    int i;
    for (i = 0; i < length; i++) {
        putchar(ch);
    }
    putchar('\n');
}
 
void removeNewline(char str[]) {
    str[strcspn(str, "\n")] = '\0';
}
 
void trimSpaces(char str[]) {
    int start = 0;
    int end = strlen(str) - 1;
    int i, j = 0;
    char temp[MAX_LINE_LEN];
 
    while (str[start] != '\0' && isspace((unsigned char)str[start])) {
        start++;
    }
 
    while (end >= start && isspace((unsigned char)str[end])) {
        end--;
    }
 
    for (i = start; i <= end; i++) {
        temp[j++] = str[i];
    }
    temp[j] = '\0';
 
    strcpy(str, temp);
}
 
void toLowerCase(char str[]) {
    int i;
    for (i = 0; str[i] != '\0'; i++) {
        str[i] = (char)tolower((unsigned char)str[i]);
    }
}
 
void normalizeQuery(const char input[], char output[]) {
    int i;
    for (i = 0; input[i] != '\0'; i++) {
        if (isalnum((unsigned char)input[i])) {
            output[i] = (char)tolower((unsigned char)input[i]);
        } else {
            output[i] = ' ';
        }
    }
    output[i] = '\0';
}
 
unsigned int hashFunction(const char *word) {
    unsigned int hash = 0;
    int i;
 
    for (i = 0; word[i] != '\0'; i++) {
        hash = (hash * 31 + (unsigned char)word[i]) % TABLE_SIZE;
    }
 
    return hash;
}
 
void initializeHashTable() {
    int i;
    for (i = 0; i < TABLE_SIZE; i++) {
        hashTable[i] = NULL;
    }
}
 
void freeHashTable() {
    int i;
    KeywordNode *current, *temp;
 
    for (i = 0; i < TABLE_SIZE; i++) {
        current = hashTable[i];
        while (current != NULL) {
            temp = current;
            current = current->next;
            free(temp);
        }
        hashTable[i] = NULL;
    }
}
 
void insertKeyword(const char *keyword, int faqIndex) {
    unsigned int index;
    KeywordNode *newNode;
 
    if (strlen(keyword) == 0) {
        return;
    }
 
    index = hashFunction(keyword);
 
    newNode = (KeywordNode *)malloc(sizeof(KeywordNode));
    if (newNode == NULL) {
        printf("Memory allocation failed while creating hash node.\n");
        return;
    }
 
    strncpy(newNode->keyword, keyword, MAX_KEYWORD_LEN - 1);
    newNode->keyword[MAX_KEYWORD_LEN - 1] = '\0';
    newNode->faqIndex = faqIndex;
 
    newNode->next = hashTable[index];
    hashTable[index] = newNode;
}
 
KeywordNode *searchKeyword(const char *keyword) {
    unsigned int index = hashFunction(keyword);
    KeywordNode *current = hashTable[index];
 
    while (current != NULL) {
        if (strcmp(current->keyword, keyword) == 0) {
            return current;
        }
        current = current->next;
    }
 
    return NULL;
}
 
void createDefaultKnowledgeBase() {
    FILE *fp = fopen(KB_FILE, "w");
 
    if (fp == NULL) {
        printf("Unable to create default knowledge base file.\n");
        return;
    }
 
    fprintf(fp, "scholarship|scholarship,bank,account,epass,jagananna,reimbursement|For scholarship issues, visit the Scholarship Section with your student ID, bank passbook copy, Aadhaar copy, and previous application details.\n");
    fprintf(fp, "exam_fee|exam,fee,challan,payment,semester,registration|For exam fee details, check the academic notice board or contact the Examination Section. Keep your student ID and payment receipt ready.\n");
    fprintf(fp, "hostel|hostel,room,dorm,warden,permission,outing|For hostel-related issues, contact the concerned hostel caretaker or warden. For outing permission, submit the request as per campus rules.\n");
    fprintf(fp, "medical|medical,hospital,doctor,medicine,health,sick,fever|For medical help, visit the campus medical room. In emergency situations, immediately inform the caretaker, security, or Administrative Office.\n");
    fprintf(fp, "library|library,book,renew,return,fine,borrow|For library services, contact the library counter with your ID card. Return or renew books before the due date to avoid fines.\n");
    fprintf(fp, "id_card|id,card,identity,lost,duplicate|For lost ID card or duplicate ID card, submit an application to the Administrative Office through the concerned section.\n");
    fprintf(fp, "wifi|wifi,internet,password,network,login,connection|For Wi-Fi or internet issues, contact the IT Infrastructure Office with your student ID, device details, and room or block location.\n");
    fprintf(fp, "transport|bus,transport,route,vehicle,driver,timing|For transport-related queries, contact the Transport Section or Administrative Office for route, timing, and permission details.\n");
    fprintf(fp, "mess|mess,food,dining,menu,quality,canteen|For mess or food-quality issues, report to the mess committee, caretaker, or Administrative Office with date, time, and exact problem details.\n");
    fprintf(fp, "office|office,administration,ao,director,section,timing|For general administrative queries, visit the Administrative Office during working hours with proper ID and supporting documents.\n");
 
    fclose(fp);
}
 
int loadKnowledgeBase() {
    FILE *fp;
    char line[MAX_LINE_LEN];
    char *intentPart, *keywordPart, *responsePart;
    char *keywordToken;
    FAQ faq;
 
    fp = fopen(KB_FILE, "r");
    if (fp == NULL) {
        return 0;
    }
 
    while (fgets(line, sizeof(line), fp) != NULL && faqCount < MAX_FAQS) {
        removeNewline(line);
 
        if (strlen(line) == 0 || line[0] == '#') {
            continue;
        }
 
        intentPart = strtok(line, "|");
        keywordPart = strtok(NULL, "|");
        responsePart = strtok(NULL, "|");
 
        if (intentPart == NULL || keywordPart == NULL || responsePart == NULL) {
            continue;
        }
 
        memset(&faq, 0, sizeof(FAQ));
 
        trimSpaces(intentPart);
        trimSpaces(keywordPart);
        trimSpaces(responsePart);
 
        strncpy(faq.intent, intentPart, MAX_INTENT_LEN - 1);
        strncpy(faq.response, responsePart, MAX_RESPONSE_LEN - 1);
 
        faq.keywordCount = 0;
        keywordToken = strtok(keywordPart, ",");
 
        while (keywordToken != NULL && faq.keywordCount < MAX_KEYWORDS) {
            trimSpaces(keywordToken);
            toLowerCase(keywordToken);
 
            if (strlen(keywordToken) > 0) {
                strncpy(faq.keywords[faq.keywordCount], keywordToken, MAX_KEYWORD_LEN - 1);
                faq.keywordCount++;
            }
 
            keywordToken = strtok(NULL, ",");
        }
 
        if (faq.keywordCount > 0) {
            faqList[faqCount] = faq;
            faqCount++;
        }
    }
 
    fclose(fp);
    return 1;
}
 
void buildHashIndex() {
    int i, j;
 
    for (i = 0; i < faqCount; i++) {
        for (j = 0; j < faqList[i].keywordCount; j++) {
            insertKeyword(faqList[i].keywords[j], i);
        }
    }
}
 
void displayAllCategories() {
    int i, j;
 
    printHeader();
    printf("\nAVAILABLE HELP DESK CATEGORIES\n");
    printLine('-', 72);
 
    if (faqCount == 0) {
        printf("No categories available.\n");
        return;
    }
 
    for (i = 0; i < faqCount; i++) {
        printf("\n%d. Intent/Category : %s\n", i + 1, faqList[i].intent);
        printf("   Keywords        : ");
        for (j = 0; j < faqList[i].keywordCount; j++) {
            printf("%s", faqList[i].keywords[j]);
            if (j < faqList[i].keywordCount - 1) {
                printf(", ");
            }
        }
        printf("\n   Response        : %s\n", faqList[i].response);
    }
}
 
void chatbotMode() {
    char query[MAX_LINE_LEN];
 
    printHeader();
    printf("\nCHATBOT MODE\n");
    printLine('-', 72);
    printf("Type your campus-related question. Type 'exit' to return to menu.\n");
    printf("Example: I lost my ID card. What should I do?\n");
    printLine('-', 72);
 
    while (1) {
        printf("\nStudent: ");
        fgets(query, sizeof(query), stdin);
        removeNewline(query);
        trimSpaces(query);
 
        if (strlen(query) == 0) {
            printf("Bot: Please type a question.\n");
            continue;
        }
 
        if (strcmp(query, "exit") == 0 || strcmp(query, "EXIT") == 0) {
            break;
        }
 
        findBestAnswer(query);
    }
}
 
void findBestAnswer(const char query[]) {
    char normalized[MAX_LINE_LEN];
    char queryCopy[MAX_LINE_LEN];
    char *word;
    int scores[MAX_FAQS];
    int i;
    int bestIndex = -1;
    int bestScore = 0;
    KeywordNode *node;
 
    for (i = 0; i < MAX_FAQS; i++) {
        scores[i] = 0;
    }
 
    normalizeQuery(query, normalized);
    strcpy(queryCopy, normalized);
 
    word = strtok(queryCopy, " ");
    while (word != NULL) {
        if (strlen(word) > 1) {
            node = searchKeyword(word);
            while (node != NULL) {
                if (strcmp(node->keyword, word) == 0) {
                    scores[node->faqIndex]++;
                }
                node = node->next;
            }
        }
        word = strtok(NULL, " ");
    }
 
    for (i = 0; i < faqCount; i++) {
        if (scores[i] > bestScore) {
            bestScore = scores[i];
            bestIndex = i;
        }
    }
 
    if (bestIndex == -1 || bestScore == 0) {
        printf("\nBot: Sorry, I could not understand this query clearly.\n");
        printf("Bot: Please try using words like scholarship, exam, hostel, medical, library, ID card, Wi-Fi, mess, or office.\n");
        logChat(query, "no_match", "No suitable response found.");
    } else {
        printf("\nBot: Category detected: %s\n", faqList[bestIndex].intent);
        printf("Bot: Match score: %d keyword(s) matched\n", bestScore);
        printf("Bot: %s\n", faqList[bestIndex].response);
        logChat(query, faqList[bestIndex].intent, faqList[bestIndex].response);
    }
}
 
void addNewFAQ() {
    FAQ faq;
    char keywordLine[MAX_LINE_LEN];
    char *keywordToken;
 
    printHeader();
    printf("\nADD NEW FAQ / KNOWLEDGE BASE ENTRY\n");
    printLine('-', 72);
 
    if (faqCount >= MAX_FAQS) {
        printf("Maximum FAQ limit reached. Cannot add more entries.\n");
        return;
    }
 
    memset(&faq, 0, sizeof(FAQ));
 
    printf("Enter intent/category name, e.g., sports, placements, certificates: ");
    fgets(faq.intent, sizeof(faq.intent), stdin);
    removeNewline(faq.intent);
    trimSpaces(faq.intent);
 
    if (strlen(faq.intent) == 0) {
        printf("Intent cannot be empty. Entry cancelled.\n");
        return;
    }
 
    printf("Enter keywords separated by comma, e.g., sports,ground,coach: ");
    fgets(keywordLine, sizeof(keywordLine), stdin);
    removeNewline(keywordLine);
    trimSpaces(keywordLine);
 
    printf("Enter chatbot response: ");
    fgets(faq.response, sizeof(faq.response), stdin);
    removeNewline(faq.response);
    trimSpaces(faq.response);
 
    if (strlen(keywordLine) == 0 || strlen(faq.response) == 0) {
        printf("Keywords and response cannot be empty. Entry cancelled.\n");
        return;
    }
 
    faq.keywordCount = 0;
    keywordToken = strtok(keywordLine, ",");
 
    while (keywordToken != NULL && faq.keywordCount < MAX_KEYWORDS) {
        trimSpaces(keywordToken);
        toLowerCase(keywordToken);
 
        if (strlen(keywordToken) > 0) {
            strncpy(faq.keywords[faq.keywordCount], keywordToken, MAX_KEYWORD_LEN - 1);
            faq.keywordCount++;
        }
 
        keywordToken = strtok(NULL, ",");
    }
 
    faqList[faqCount] = faq;
    appendFAQToFile(faq);
 
    for (int i = 0; i < faq.keywordCount; i++) {
        insertKeyword(faq.keywords[i], faqCount);
    }
 
    faqCount++;
 
    printf("\nNew FAQ added successfully and saved into '%s'.\n", KB_FILE);
}
 
void appendFAQToFile(FAQ faq) {
    FILE *fp;
    int i;
 
    fp = fopen(KB_FILE, "a");
    if (fp == NULL) {
        printf("Unable to save FAQ into file.\n");
        return;
    }
 
    fprintf(fp, "%s|", faq.intent);
    for (i = 0; i < faq.keywordCount; i++) {
        fprintf(fp, "%s", faq.keywords[i]);
        if (i < faq.keywordCount - 1) {
            fprintf(fp, ",");
        }
    }
    fprintf(fp, "|%s\n", faq.response);
 
    fclose(fp);
}
 
void logChat(const char query[], const char intent[], const char response[]) {
    FILE *fp;
    time_t now;
    char *timeText;
 
    fp = fopen(LOG_FILE, "a");
    if (fp == NULL) {
        return;
    }
 
    now = time(NULL);
    timeText = ctime(&now);
    if (timeText != NULL) {
        removeNewline(timeText);
    }
 
    fprintf(fp, "Time: %s\n", timeText != NULL ? timeText : "unknown");
    fprintf(fp, "Query: %s\n", query);
    fprintf(fp, "Detected Intent: %s\n", intent);
    fprintf(fp, "Response: %s\n", response);
    fprintf(fp, "--------------------------------------------------\n");
 
    fclose(fp);
}
 
void showProjectInfo() {
    printHeader();
    printf("\nPROJECT INFORMATION\n");
    printLine('-', 72);
    printf("Project Title : Campus Help Desk Chatbot in C\n");
    printf("AI Type       : Rule-based / Keyword-based chatbot\n");
    printf("Data Structure: Hash table with separate chaining\n");
    printf("File Handling : Knowledge base file and chat log file\n");
    printf("Use Case      : Answer common campus questions quickly\n");
    printf("Users         : Students, help desk staff, admin office, hostel office\n");
    printf("\nThis project is not a generative AI system. It does not create new answers by itself.\n");
    printf("It searches important keywords and returns the best matching predefined answer.\n");
}
 
void showHelp() {
    printHeader();
    printf("\nHELP / HOW TO ASK QUESTIONS\n");
    printLine('-', 72);
    printf("Ask short campus-related questions using important keywords.\n\n");
    printf("Good examples:\n");
    printf("  - I lost my ID card\n");
    printf("  - Scholarship bank account problem\n");
    printf("  - Wi-Fi is not working in my hostel block\n");
    printf("  - I have fever and need medical help\n");
    printf("  - Exam fee payment last date\n");
    printf("\nPoor examples:\n");
    printf("  - help me\n");
    printf("  - what to do\n");
    printf("  - problem\n");
    printf("\nReason: The chatbot depends on keyword matching. Clear keywords give better answers.\n");
}
 
int getMenuChoice() {
    char input[20];
    int choice;
 
    while (1) {
        printf("Enter your choice (1-7): ");
        fgets(input, sizeof(input), stdin);
 
        if (sscanf(input, "%d", &choice) == 1 && choice >= 1 && choice <= 7) {
            return choice;
        }
 
        printf("Invalid choice. Please enter a number between 1 and 7.\n");
    }
}
