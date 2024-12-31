#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <regex.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_USERS 100
#define MAX_ROOMS 50
#define MAX_BOOKINGS 100
#define FILENAME_USERS "users.dat"
#define FILENAME_ROOMS "rooms.dat"
#define FILENAME_BOOKINGS "bookings.dat"

typedef struct {
  int id;
  char username[50];
  char password[50];
  char role[20];
  char department[50];
  char name[100];
  char email[100];
  char phone[20];
  char address[200];
} User;

typedef struct {
  int id;
  char type[50];
  float price;
  int capacity;
  int status;
  char amenities[200];
} Room;

typedef struct {
  int id;
  int userId;
  int roomId;
  char checkIn[20];
  char checkOut[20];
  double totalAmount;
  int status;
} Booking;

// Function to check if a string is a valid integer within the allowed range
bool isValidInteger(const char *str, int *value) {
  char *endptr;

  // Convert string to long
  errno = 0; // Reset errno
  long num = strtol(str, &endptr, 10);

  // Check for conversion errors
  if ((errno == ERANGE && (num == LONG_MAX || num == LONG_MIN)) ||
      (errno != 0 && num == 0)) {
    perror("strtol");
    return false;
  }

  // Check if the entire string was consumed
  if (*endptr != '\0') {
    return false; // Not a valid integer
  }

  // Check for range (INT_MIN to INT_MAX)
  if (num < INT_MIN || num > INT_MAX) {
    return false;
  }

  // Store the converted integer
  *value = (int)num;

  return true;
}

// Function to validate an integer input with range check and buffer overflow
// prevention
bool validateInteger(const char *valueName, int minValue, int maxValue,
                     int *userInput) {
  char buffer[100]; // Buffer to store input string (adjust size as needed)

  printf("Enter %s (between %d and %d): ", valueName, minValue, maxValue);

  // Read input using fgets to prevent buffer overflow
  if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
    fprintf(stderr, "Error reading input.\n");
    return false;
  }

  // Remove the trailing newline character from fgets
  buffer[strcspn(buffer, "\n")] = 0;

  // Convert and validate the integer
  if (!isValidInteger(buffer, userInput)) {
    printf("Invalid %s. Please enter a valid integer.\n", valueName);
    return false;
  }

  // Check if the integer is within the allowed range
  if (*userInput < minValue || *userInput > maxValue) {
    printf("%s must be between %d and %d.\n", valueName, minValue, maxValue);
    return false;
  }

  return true;
}

// Function to check if a string is a valid positive integer
bool isValidPositiveInteger(const char *str, int *value) {
  char *endptr;

  // Convert string to long
  errno = 0;
  long num = strtol(str, &endptr, 10);

  // Check for conversion errors
  if ((errno == ERANGE && (num == LONG_MAX || num == LONG_MIN)) ||
      (errno != 0 && num == 0)) {
    perror("strtol");
    return false;
  }

  // Check if the entire string was consumed
  if (*endptr != '\0') {
    return false;
  }

  // Check for positive value and range
  if (num <= 0 || num > INT_MAX) {
    return false;
  }

  *value = (int)num;
  return true;
}

// Function to validate a positive integer input
bool validatePositiveInteger(const char *valueName, int *userInput) {
  char buffer[100];

  printf("Enter %s (positive integer): ", valueName);

  if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
    fprintf(stderr, "Error reading input.\n");
    return false;
  }

  buffer[strcspn(buffer, "\n")] = 0;

  if (!isValidPositiveInteger(buffer, userInput)) {
    printf("Invalid %s. Please enter a positive integer.\n", valueName);
    return false;
  }

  return true;
}

// Function to check if a string is a valid positive float
bool isValidPositiveFloat(const char *str, float *value) {
  char *endptr;

  // Convert string to float
  errno = 0;
  float num = strtof(str, &endptr);

  // Check for conversion errors
  if ((errno == ERANGE && (num == FLT_MAX || num == FLT_MIN)) ||
      (errno != 0 && num == 0)) {
    perror("strtof");
    return false;
  }

  // Check if the entire string was consumed
  if (*endptr != '\0') {
    return false;
  }

  // Check for positive value
  if (num <= 0) {
    return false;
  }

  *value = num;
  return true;
}

// Function to validate a positive float input
bool validatePositiveFloat(const char *valueName, float *userInput) {
  char buffer[100];

  printf("Enter %s (positive float): ", valueName);

  if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
    fprintf(stderr, "Error reading input.\n");
    return false;
  }

  buffer[strcspn(buffer, "\n")] = 0;

  if (!isValidPositiveFloat(buffer, userInput)) {
    printf("Invalid %s. Please enter a positive floating-point number.\n",
           valueName);
    return false;
  }

  return true;
}

bool isWithinRange(const char *str, int minLength, int maxLength) {
  int len = strlen(str);
  return len >= minLength && len <= maxLength;
}

// Function to validate a date string in YYYY-MM-DD format
bool isValidDate(const char *dateStr) {
  struct tm tm;

  // Check if date string is the right length
  if (!isWithinRange(dateStr, 10, 10)) {
    return false;
  }

  // Try to parse the date string
  if (sscanf(dateStr, "%d-%d-%d", &tm.tm_year, &tm.tm_mon, &tm.tm_mday) != 3) {
    return false;
  }

  // Adjust year and month for struct tm
  tm.tm_year -= 1900;
  tm.tm_mon -= 1;
  tm.tm_isdst = -1;

  // Check if the date is valid using mktime
  time_t t = mktime(&tm);
  if (t == -1) {
    return false;
  }

  return true;
}

bool validateBookingDates(const char *checkInStr, const char *checkOutStr) {
  if (!isValidDate(checkInStr) || !isValidDate(checkOutStr)) {
    printf("Invalid date format. Please use YYYY-MM-DD.\n");
    return false;
  }

  struct tm checkInTm = {0}, checkOutTm = {0};

  // Parse the date strings using strptime
  if (!strptime(checkInStr, "%Y-%m-%d", &checkInTm)) {
    printf("Error parsing check-in date.\n");
    return false;
  }
  if (!strptime(checkOutStr, "%Y-%m-%d", &checkOutTm)) {
    printf("Error parsing check-out date.\n");
    return false;
  }

  // Set time to 0 to compare only dates
  checkInTm.tm_hour = 0;
  checkInTm.tm_min = 0;
  checkInTm.tm_sec = 0;
  checkOutTm.tm_hour = 0;
  checkOutTm.tm_min = 0;
  checkOutTm.tm_sec = 0;

  time_t checkInTime = mktime(&checkInTm);
  time_t checkOutTime = mktime(&checkOutTm);

  if (checkOutTime <= checkInTime) {
    printf("Check-out date must be after check-in date.\n");
    return false;
  }

  return true;
}

bool validateString(const char *stringName, int minLength, int maxLength,
                    char *userInput, size_t inputSize) {
  printf("Enter %s (between %d and %d characters): ", stringName, minLength,
         maxLength);

  // Read a line of input using fgets
  if (fgets(userInput, inputSize, stdin) == NULL) {
    fprintf(stderr, "Error reading input.\n");
    return false;
  }

  // Remove the trailing newline character from fgets
  userInput[strcspn(userInput, "\n")] = 0;

  if (!isWithinRange(userInput, minLength, maxLength)) {
    printf("%s must be between %d to %d characters long.\n", stringName,
           minLength, maxLength);
    return false;
  }

  return true;
}

// Function to validate an email address using regex
bool isValidEmail(const char *email) {
  regex_t regex;
  int reti;
  char msgbuf[100];

  // Regex Compilation:
  reti = regcomp(&regex, "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$",
                 REG_EXTENDED);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    return false;
  }

  // Regex Execution:
  reti = regexec(&regex, email, 0, NULL, 0);
  if (!reti) {
    regfree(&regex);
    return true; // Match found
  } else if (reti == REG_NOMATCH) {
    regfree(&regex);
    return false; // No match
  } else {
    regerror(reti, &regex, msgbuf, sizeof(msgbuf));
    fprintf(stderr, "Regex match failed: %s\n", msgbuf);
    regfree(&regex);
    return false;
  }
}

bool validatePhone(const char *phone, char *validatedPhone, size_t bufferSize) {
  if (!validateString("Phone", 10, 11, validatedPhone,
                      bufferSize)) { // Adjust length as needed
    return false;
  }

  int digitCount = 0;
  for (int i = 0; validatedPhone[i] != '\0'; i++) {
    if (isdigit(validatedPhone[i])) {
      digitCount++;
    }
  }

  if (digitCount != 10 || digitCount != 11) {
    printf(
        "Invalid phone number format. Please enter a 10 or 11 digit number.\n");
    return false;
  }

  return true;
}

User users[MAX_USERS];
Room rooms[MAX_ROOMS];
Booking bookings[MAX_BOOKINGS];
int userCount = 0;
int roomCount = 0;
int bookingCount = 0;
User currentUser;

bool isUsernameTaken(const char *username) {
  for (int i = 0; i < userCount; i++) {
    if (strcmp(users[i].username, username) == 0) {
      return true;
    }
  }
  return false;
}

bool validateUsername(const char *username, char *validatedUsername,
                      size_t bufferSize) {
  if (!validateString("Username", 5, 49, validatedUsername, bufferSize)) {
    return false;
  }

  // Check if the first character is a letter
  if (!isalpha(validatedUsername[0])) {
    printf("Username must start with a letter.\n");
    return false;
  }

  // Check for allowed characters
  for (int i = 0; validatedUsername[i] != '\0'; i++) {
    if (!isalnum(validatedUsername[i]) && validatedUsername[i] != '_' &&
        validatedUsername[i] != '.') {
      printf("Username can only contain alphanumeric characters, underscores, "
             "and dots.\n");
      return false;
    }
  }

  if (isUsernameTaken(validatedUsername)) {
    printf("Username already taken.\n");
    return false;
  }

  return true;
}

bool validateUserId(int userId) {
  for (int i = 0; i < userCount; i++) {
    if (users[i].id == userId) {
      return true;
    }
  }
  printf("User with ID %d not found.\n", userId);
  return false;
}

bool validateRoomId(int roomId) {
  for (int i = 0; i < roomCount; i++) {
    if (rooms[i].id == roomId) {
      return true; // Room ID found
    }
  }
  printf("Room with ID %d not found.\n", roomId);
  return false; // Room ID not found
}

// Function to validate the user role
bool validateRole(const char *role) {
  return (strcmp(role, "admin") == 0 || strcmp(role, "employee") == 0 ||
          strcmp(role, "customer") == 0);
}

bool isRoomAvailable(int roomId, const char *checkIn, const char *checkOut) {
  for (int i = 0; i < bookingCount; i++) {
    if (bookings[i].roomId == roomId &&
        bookings[i].status == 1) { // Assuming status 1 means active booking
      // Check for date overlap
      if (validateBookingDates(checkIn, bookings[i].checkOut) &&
          validateBookingDates(bookings[i].checkIn, checkOut)) {
        printf("Room is not available for those dates.\n");
        return false;
      }
    }
  }
  return true; // Room is available
}

bool validateDepartment(const char *department) {
  const char *allowedDepartments[] = {"HR", "Sales", "IT", "Front Desk",
                                      "Management"};
  const int numDepartments =
      sizeof(allowedDepartments) / sizeof(allowedDepartments[0]);

  for (int i = 0; i < numDepartments; i++) {
    if (strcmp(department, allowedDepartments[i]) == 0) {
      return true; // Valid department
    }
  }

  printf("Invalid department. Allowed departments are: ");
  for (int i = 0; i < numDepartments; i++) {
    printf("%s", allowedDepartments[i]);
    if (i < numDepartments - 1) {
      printf(", ");
    }
  }
  printf(".\n");
  return false;
}

void loadData() {
  FILE *fp;

  fp = fopen(FILENAME_USERS, "rb");
  if (fp != NULL) {
    userCount = fread(users, sizeof(User), MAX_USERS, fp);
    fclose(fp);
  }

  fp = fopen(FILENAME_ROOMS, "rb");
  if (fp != NULL) {
    roomCount = fread(rooms, sizeof(Room), MAX_ROOMS, fp);
    fclose(fp);
  }

  fp = fopen(FILENAME_BOOKINGS, "rb");
  if (fp != NULL) {
    bookingCount = fread(bookings, sizeof(Booking), MAX_BOOKINGS, fp);
    fclose(fp);
  }
}

void saveData() {
  FILE *fp;

  fp = fopen(FILENAME_USERS, "wb");
  if (fp != NULL) {
    fwrite(users, sizeof(User), userCount, fp);
    fclose(fp);
  }

  fp = fopen(FILENAME_ROOMS, "wb");
  if (fp != NULL) {
    fwrite(rooms, sizeof(Room), roomCount, fp);
    fclose(fp);
  }

  fp = fopen(FILENAME_BOOKINGS, "wb");
  if (fp != NULL) {
    fwrite(bookings, sizeof(Booking), bookingCount, fp);
    fclose(fp);
  }
}

int login(char *username, char *password) {
  for (int i = 0; i < userCount; i++) {
    if (strcmp(users[i].username, username) == 0 &&
        strcmp(users[i].password, password) == 0) {
      currentUser = users[i];
      return 1;
    }
  }
  return 0;
}

void registerUser() {
  User newUser;
  char inputBuffer[200];

  printf("\nRegistration Form\n");

  while (true) {
    if (validateUsername(newUser.username, newUser.username,
                         sizeof(newUser.username))) {
      break;
    }
  }

  while (true) {
    if (validateString("Password", 5, 49, newUser.password,
                       sizeof(newUser.password))) {
      break;
    }
  }

  while (true) {
    if (validateString("Role (admin/employee/customer)", 1, 19, inputBuffer,
                       sizeof(inputBuffer))) {
      if (validateRole(inputBuffer)) {
        strncpy(newUser.role, inputBuffer, sizeof(newUser.role));
        break;
      } else {
        printf(
            "Invalid role. Please enter 'admin', 'employee', or 'customer'.\n");
      }
    }
  }

  if (strcmp(newUser.role, "employee") == 0) {
    while (true) {
      if (validateString("Department", 1, 49, inputBuffer,
                         sizeof(inputBuffer))) {
        if (validateDepartment(inputBuffer)) {
          strncpy(newUser.department, inputBuffer, sizeof(newUser.department));
          break;
        }
      }
    }
  }

  while (true) {
    if (validateString("Name", 1, 99, newUser.name, sizeof(newUser.name))) {
      break;
    }
  }

  while (true) {
    if (validateString("Email", 1, 99, inputBuffer, sizeof(inputBuffer))) {
      if (isValidEmail(inputBuffer)) {
        strncpy(newUser.email, inputBuffer, sizeof(newUser.email));
        break;
      } else {
        printf("Invalid email format.\n");
      }
    }
  }

  while (true) {
    if (validatePhone(newUser.phone, newUser.phone, sizeof(newUser.phone))) {
      break;
    }
  }

  while (true) {
    if (validateString("Address", 1, 199, newUser.address,
                       sizeof(newUser.address))) {
      break;
    }
  }

  newUser.id = userCount + 1;
  users[userCount++] = newUser;
  saveData();
  printf("\nRegistration successful!\n");
}

void addRoom() {
  if (roomCount >= MAX_ROOMS) {
    printf("\nMaximum room capacity reached!\n");
    return;
  }

  Room newRoom;
  printf("\nAdd New Room\n");

  while (true) {
    if (validateString("Room Type", 1, 49, newRoom.type,
                       sizeof(newRoom.type))) {
      break;
    }
  }

  while (true) {
    if (validatePositiveFloat("Price per night", &newRoom.price)) {
      break;
    }
  }

  while (true) {
    if (validatePositiveInteger("Capacity", &newRoom.capacity)) {
      break;
    }
  }

  while (true) {
    if (validateString("Amenities", 0, 199, newRoom.amenities,
                       sizeof(newRoom.amenities))) {
      break;
    }
  }

  newRoom.id = roomCount + 1;
  newRoom.status = 1; // 1 for available, 0 for booked
  rooms[roomCount++] = newRoom;
  saveData();
  printf("\nRoom added successfully!\n");
}

void listRooms() {
  printf("\nRoom List\n");
  printf("%-8s%-12s%-10s%-10s%-10s%s\n", "ID", "Type", "Price", "Capacity",
         "Status", "Amenities");
  for (int i = 0; i < roomCount; i++) {
    printf("%-8d%-12s%-10.2f%-10d%-10s%s\n", rooms[i].id, rooms[i].type,
           rooms[i].price, rooms[i].capacity,
           rooms[i].status ? "Available" : "Booked", rooms[i].amenities);
  }
}

void updateRoom() {
  int id;
  printf("\nEnter Room ID to update: ");

  while (true) {
    if (validatePositiveInteger("Room ID", &id)) {
      if (validateRoomId(id)) {
        break;
      }
    }
  }

  for (int i = 0; i < roomCount; i++) {
    if (rooms[i].id == id) {
      while (true) {
        if (validateString("New Room Type (current: %s)", 1, 49, rooms[i].type,
                           sizeof(rooms[i].type))) {
          break;
        }
      }

      while (true) {
        if (validatePositiveFloat("New Price (current: %.2f)",
                                  &rooms[i].price)) {
          break;
        }
      }

      while (true) {
        if (validatePositiveInteger("New Capacity (current: %d)",
                                    &rooms[i].capacity)) {
          break;
        }
      }

      while (true) {
        if (validateString("New Amenities (current: %s)", 0, 199,
                           rooms[i].amenities, sizeof(rooms[i].amenities))) {
          break;
        }
      }

      saveData();
      printf("\nRoom updated successfully!\n");
      return;
    }
  }
  printf("\nRoom not found!\n");
}

void deleteRoom() {
  int id;
  printf("\nEnter Room ID to delete: ");

  while (true) {
    if (validatePositiveInteger("Room ID", &id)) {
      if (validateRoomId(id)) {
        break;
      }
    }
  }

  for (int i = 0; i < roomCount; i++) {
    if (rooms[i].id == id) {
      for (int j = i; j < roomCount - 1; j++) {
        rooms[j] = rooms[j + 1];
      }
      roomCount--;
      saveData();
      printf("\nRoom deleted successfully!\n");
      return;
    }
  }
  printf("\nRoom not found!\n");
}

// Manual parsing of date strings to struct tm
void parseDate(const char *dateStr, struct tm *date) {
  sscanf(dateStr, "%d-%d-%d", &date->tm_year, &date->tm_mon, &date->tm_mday);
  date->tm_mon -= 1;     // tm_mon is 0-11 in struct tm
  date->tm_year -= 1900; // tm_year is years since 1900 in struct tm
}

void calculateTotalAmount(Booking *booking) {
  struct tm checkIn = {0}, checkOut = {0};
  time_t checkInTime, checkOutTime;
  double seconds;

  // Manually parse the date strings into struct tm
  parseDate(booking->checkIn, &checkIn);
  parseDate(booking->checkOut, &checkOut);

  // Convert struct tm to time_t
  checkInTime = mktime(&checkIn);
  checkOutTime = mktime(&checkOut);

  if (checkInTime == -1 || checkOutTime == -1) {
    fprintf(stderr, "Error converting dates to time_t.\n");
    return;
  }

  // Calculate the difference in seconds
  seconds = difftime(checkOutTime, checkInTime);

  // Convert seconds to days using long long
  long long days = seconds / (60 * 60 * 24);

  if (days < 0) {
    days = 0; // Handle negative days (you might want to prevent this earlier)
  }

  // Find the room and calculate total amount
  bool roomFound = false;
  for (int i = 0; i < roomCount; i++) {
    if (rooms[i].id == booking->roomId) {
      booking->totalAmount = (double)rooms[i].price * days;
      roomFound = true;
      break;
    }
  }

  if (!roomFound) {
    fprintf(stderr, "Error: Room with ID %d not found.\n", booking->roomId);
  }
}

void makeBooking() {
  if (bookingCount >= MAX_BOOKINGS) {
    printf("\nMaximum booking capacity reached!\n");
    return;
  }

  Booking newBooking;
  printf("\nNew Booking\n");
  listRooms();
  printf("\nEnter Room ID: ");

  while (true) {
    if (validatePositiveInteger("Room ID", &newBooking.roomId)) {
      if (validateRoomId(newBooking.roomId)) {
        break;
      }
    }
  }

  char checkInStr[20], checkOutStr[20];
  while (true) {
    if (validateString("Check-in date (YYYY-MM-DD)", 10, 10, checkInStr,
                       sizeof(checkInStr))) {
      if (validateString("Check-out date (YYYY-MM-DD)", 10, 10, checkOutStr,
                         sizeof(checkOutStr))) {
        if (validateBookingDates(checkInStr, checkOutStr)) {
          if (isValidDate(checkInStr) && isValidDate(checkOutStr)) {
            if (isRoomAvailable(newBooking.roomId, checkInStr, checkOutStr)) {
              strncpy(newBooking.checkIn, checkInStr,
                      sizeof(newBooking.checkIn));
              strncpy(newBooking.checkOut, checkOutStr,
                      sizeof(newBooking.checkOut));
              break;
            }
          }
        }
      }
    }
  }

  // Find the room and update its status
  bool roomFound = false;
  for (int i = 0; i < roomCount; i++) {
    if (rooms[i].id == newBooking.roomId) {
      if (!rooms[i].status) {
        printf("\nRoom is already booked!\n");
        return;
      }
      rooms[i].status = 0; // Mark room as booked
      roomFound = true;
      break;
    }
  }

  if (!roomFound) {
    printf("Error: Room not found.\n");
    return;
  }

  // Set booking details
  newBooking.id = bookingCount + 1;
  newBooking.userId = currentUser.id;
  newBooking.status = 1;

  // Add the booking to the array and increment the count
  bookings[bookingCount++] = newBooking;

  // Save data immediately after updating the room status and adding the booking
  saveData();

  // Calculate the total amount after saving the data
  calculateTotalAmount(
      &bookings[bookingCount - 1]); // Use bookingCount - 1 as the index

  printf("\nBooking successful!\n");
  printf("Total amount: $%.2f\n", bookings[bookingCount - 1].totalAmount);
}

void viewBookings() {
  printf("\nBooking List\n");
  printf("ID\tRoom\tCheck-in\tCheck-out\tAmount\tStatus\n");
  for (int i = 0; i < bookingCount; i++) {
    if (strcmp(currentUser.role, "customer") == 0 &&
        bookings[i].userId != currentUser.id) {
      continue;
    }
    printf("%d\t%d\t%s\t%s\t%.2f\t%s\n", bookings[i].id, bookings[i].roomId,
           bookings[i].checkIn, bookings[i].checkOut, bookings[i].totalAmount,
           bookings[i].status ? "Active" : "Completed");
  }
}

void viewCustomers() {
  printf("\nCustomer List\n");
  printf("ID\tName\tEmail\tPhone\tAddress\n");
  for (int i = 0; i < userCount; i++) {
    if (strcmp(users[i].role, "customer") == 0) {
      printf("%d\t%s\t%s\t%s\t%s\n", users[i].id, users[i].name, users[i].email,
             users[i].phone, users[i].address);
    }
  }
}

void adminMenu() {
  int choice;
  do {
    printf("\nAdmin Menu\n");
    printf("1. Add Room\n");
    printf("2. List Rooms\n");
    printf("3. Update Room\n");
    printf("4. Delete Room\n");
    printf("5. View Customers\n");
    printf("6. View Bookings\n");
    printf("7. Logout\n");
    printf("Choice: ");

    while (true) {
      if (validateInteger("Choice", 1, 7, &choice)) {
        break;
      }
    }

    switch (choice) {
    case 1:
      addRoom();
      break;
    case 2:
      listRooms();
      break;
    case 3:
      updateRoom();
      break;
    case 4:
      deleteRoom();
      break;
    case 5:
      viewCustomers();
      break;
    case 6:
      viewBookings();
      break;
    case 7:
      printf("\nLogging out...\n");
      break;
    default:
      printf("\nInvalid choice!\n");
    }
  } while (choice != 7);
}

void employeeMenu() {
  int choice;
  do {
    printf("\nEmployee Menu\n");
    printf("1. View Customers\n");
    printf("2. View Rooms\n");
    printf("3. View Bookings\n");
    printf("4. Logout\n");
    printf("Choice: ");

    while (true) {
      if (validateInteger("Choice", 1, 4, &choice)) {
        break;
      }
    }

    switch (choice) {
    case 1:
      viewCustomers();
      break;
    case 2:
      listRooms();
      break;
    case 3:
      viewBookings();
      break;
    case 4:
      printf("\nLogging out...\n");
      break;
    default:
      printf("\nInvalid choice!\n");
    }
  } while (choice != 4);
}

void customerMenu() {
  int choice;
  do {
    printf("\nCustomer Menu\n");
    printf("1. View Rooms\n");
    printf("2. Make Booking\n");
    printf("3. View My Bookings\n");
    printf("4. Logout\n");
    printf("Choice: ");

    while (true) {
      if (validateInteger("Choice", 1, 4, &choice)) {
        break;
      }
    }

    switch (choice) {
    case 1:
      listRooms();
      break;
    case 2:
      makeBooking();
      break;
    case 3:
      viewBookings();
      break;
    case 4:
      printf("\nLogging out...\n");
      break;
    default:
      printf("\nInvalid choice!\n");
    }
  } while (choice != 4);
}

int main() {
  loadData();
  int choice;
  char username[50], password[50];

  do {
    printf("\nHotel Management System\n");
    printf("1. Login\n");
    printf("2. Register\n");
    printf("3. Exit\n");
    printf("Choice: ");

    while (true) {
      if (validateInteger("Choice", 1, 3, &choice)) {
        break;
      }
    }

    switch (choice) {
    case 1:
      printf("\nLogin\n");
      printf("Username: ");
      fgets(username, sizeof(username), stdin);
      username[strcspn(username, "\n")] = 0; // Remove trailing newline

      printf("Password: ");
      fgets(password, sizeof(password), stdin);
      password[strcspn(password, "\n")] = 0; // Remove trailing newline

      if (login(username, password)) {
        printf("\nLogin successful!\n");
        if (strcmp(currentUser.role, "admin") == 0) {
          adminMenu();
        } else if (strcmp(currentUser.role, "employee") == 0) {
          employeeMenu();
        } else {
          customerMenu();
        }
      } else {
        printf("\nInvalid credentials!\n");
      }
      break;

    case 2:
      registerUser();
      break;

    case 3:
      printf("\nThank you for using our system!\n");
      break;

    default:
      printf("\nInvalid choice!\n");
    }
  } while (choice != 3);

  return 0;
}
