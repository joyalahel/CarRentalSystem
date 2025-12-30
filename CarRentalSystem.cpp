#include <iostream>
#include <openssl/sha.h>
#include <string>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <vector>
using namespace std;
struct date {
	string startDate;
	string endDate;
};
struct car {
	string plateNumber;
	string brand;
	string model;
	int year;
	string color;
	double pricePerDay;
	date d;
	bool isAvailable = true;
};
struct client {
	int usertID;
	string firstName;
	string lastName;
	string password;
	string phone;
	string email;
	int nbReservation;
	car* c;
};
bool isValidPassword(const char* password)
{
	if (strlen(password) < 8) {
		return false;
	}
	bool hasLetter = false;
	bool hasDigit = false;
	bool hasSpecial = false;
	for (int i = 0; i < strlen(password); i++) {
		char c = password[i];

		if (isalpha(c)) {
			hasLetter = true;
		}
		else if (isdigit(c)) {
			hasDigit = true;
		}
		else if (!iscntrl(c) && ispunct(c)) {
			hasSpecial = true;
		}

		if (hasLetter && hasDigit && hasSpecial) {
			return true;
		}
	}
	return false;
}
bool isValidName(const char* name) {
	bool hasLetter = false;
	for (int i = 0; i < strlen(name); i++) {
		char c = name[i];

		if (isalpha(c)) {
			hasLetter = true;
		}
		else
			return false;
	}
}
bool isValidPhoneNumber(const string& phoneNumber) {
	int digitCount = 0;
	for (int i = 0; i < phoneNumber.length(); ++i) {
		char c = phoneNumber[i];
		if (isdigit(c)) {
			digitCount++;
		}
		else if (c != ' ' && c != '-' && c != '(' && c != ')' && c != '+') {
			return false;
		}
	}
	return digitCount == 8;
}
bool isValidEmail(const std::string& email) {
	// Check if the email contains an '@' character
	size_t atPos = email.find('@');
	if (atPos == std::string::npos || atPos == 0 || atPos == email.length() - 1) {
		return false;
	}
	// Check if the email contains a '.' character after the '@'
	size_t dotPos = email.find('.', atPos);
	if (dotPos == std::string::npos || dotPos == atPos + 1 || dotPos == email.length() - 1) {
		return false;
	}
	// Check if the email contains only valid characters
	for (char c : email) {
		if (!(isalnum(c) || c == '@' || c == '.' || c == '-' || c == '_')) {
			return false;
		}
	}
	return true;
}
int findUserByIDAndPassword(int userID, const string& pass) {
	ifstream file("users.txt");
	if (file.is_open()) {
		int ID;
		string fname, lname, storedHashedPass, num, email; // Change the variable name to indicate it's already hashed
		while (file >> ID >> fname >> lname >> storedHashedPass >> num >> email) {
			if (ID == userID && storedHashedPass == pass) { // Compare with stored hashed password
				file.close();
				return ID;
			}
		}
		file.close();
	}
	else {
		cout << "Unable to open file." << endl;
	}
	return -1; // User not found
}

bool isValidYear(int year) {
	return year >= 1000 && year <= 9999;
}
bool isDateAfterCarYear(int carYear, const string& date) {
	// Extract year from the date string
	int year = stoi(date.substr(6, 4));
	return year > carYear;
}
bool isValidDate(const std::string& dateStr) {
	if (dateStr.size() != 10) // Check if the string length is exactly 10 characters
		return false;

	// Check if characters at position 2 and 5 are dashes
	if (dateStr[2] != '-' || dateStr[5] != '-')
		return false;

	// Extract day, month, and year as substrings
	string dayStr = dateStr.substr(0, 2);
	string monthStr = dateStr.substr(3, 2);
	string yearStr = dateStr.substr(6, 4);

	// Convert substrings to integers
	int day, month, year;
	try {
		day = stoi(dayStr);
		month = stoi(monthStr);
		year = stoi(yearStr);
	}
	catch (const invalid_argument& e) {
		return false; // Conversion failed
	}
	catch (const out_of_range& e) {
		return false; // Value out of range
	}

	// Check if day, month, and year values are valid
	if (day < 1 || day > 31 || month < 1 || month > 12 || year < 0)
		return false;

	// Validate days in each month
	if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
		return false;
	else if (month == 2) { // February
		bool isLeapYear = (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
		if ((isLeapYear && day > 29) || (!isLeapYear && day > 28))
			return false;
	}

	return true; // Valid date
}
bool doesCarExist(const string& plateNumber) {
	ifstream outFile2("cars.txt");
	if (!outFile2) {
		return false;
	}

	string line;
	while (getline(outFile2, line)) {
		if (line.find(plateNumber) != string::npos) {
			outFile2.close();
			return true;
		}
	}

	outFile2.close();
	return false;
}
bool isEndDateAfterStartDate(const string& startDate, const string& endDate) {
	tm tm_start = {};
	tm tm_end = {};
	istringstream ss_start(startDate);
	istringstream ss_end(endDate);

	// Parse start date
	if (!(ss_start >> get_time(&tm_start, "%d-%m-%Y"))) {
		cout << "Invalid start date format\n";
		return false;
	}

	// Parse end date
	if (!(ss_end >> get_time(&tm_end, "%d-%m-%Y"))) {
		cout << "Invalid end date format\n";
		return false;
	}

	// Convert tm structures to time_t
	time_t start_time = mktime(&tm_start);
	time_t end_time = mktime(&tm_end);

	// Compare time_t values
	if (start_time == -1 || end_time == -1) {
		cout << "Invalid time\n";
		return false;
	}

	return end_time > start_time;
}
bool removeCar(const string& plateNumber) {
	// Open the cars.txt file in read-write mode
	fstream file("cars.txt", ios::in | ios::out);
	if (!file) {
		cout << "Error opening cars.txt file for reading and writing." << endl;
		return false;
	}

	// Create a stringstream to store the modified content
	stringstream modifiedContent;

	bool carRemoved = false;
	string line;
	while (getline(file, line)) {
		if (line.find(plateNumber) == string::npos) {
			// Write the line to the stringstream if it does not contain the plate number
			modifiedContent << line << endl;
		}
		else {
			carRemoved = true;
		}
	}

	// Close the file
	file.close();

	if (!carRemoved) {
		cerr << "Car not found." << endl;
		return false;
	}

	// Reopen the cars.txt file in write mode to overwrite the original file
	file.open("cars.txt", ios::out | ios::trunc);
	if (!file) {
		cout << "Error opening cars.txt file for writing." << endl;
		return false;
	}

	// Write the modified content from the stringstream to the file
	file << modifiedContent.str();

	// Close the file
	file.close();

	return true;
}
void modifyCar() {
	string plateNumber;
	cout << "Enter the plate number of the car to modify: ";
	cin >> plateNumber;

	// First, remove the car if it exists
	if (!removeCar(plateNumber)) {
		cerr << "Failed to modify car. Exiting function." << endl;
		return;
	}

	// Open the cars.txt file in read-write mode
	fstream file("cars.txt", ios::in | ios::out | ios::app);
	if (!file) {
		cerr << "Error opening cars.txt file for reading and writing." << endl;
		return;
	}

	// Move the file pointer to the end of the file
	file.seekp(0, ios::end);

	// Get car details to modify
	car newCarData;
	newCarData.plateNumber = plateNumber;
	cout << "Enter new car details:" << endl;
	do {
		cout << "Brand: ";
		cin >> newCarData.brand;
	} while (!isValidName(newCarData.brand.c_str()));

	cout << "Model: ";
	cin >> newCarData.model;

	do {
		cout << "Year: ";
		cin >> newCarData.year;
	} while (!isValidYear(newCarData.year));

	do {
		cout << "Color: ";
		cin >> newCarData.color;
	} while (!isValidName(newCarData.color.c_str()));

	do {
		cout << "Price Per Day: ";
		cin >> newCarData.pricePerDay;
	} while (newCarData.pricePerDay <= 0);

	cout << "Enter a date in the format dd-mm-yyyy:" << endl;
	do {
		// Prompt user for start date until it's valid
		do {
			cout << "Start Date (dd-mm-yyyy): ";
			cin >> newCarData.d.startDate;
			if (!isValidDate(newCarData.d.startDate)) {
				cout << "Invalid start date format.\n";
			}
			else if (!isDateAfterCarYear(newCarData.year, newCarData.d.startDate)) {
				cout << "Start date must be after the year of the car.\n";
			}
		} while (!isValidDate(newCarData.d.startDate) || !isDateAfterCarYear(newCarData.year, newCarData.d.startDate));

		// Prompt user for end date until it's valid and after the start date
		do {
			cout << "End Date (dd-mm-yyyy): ";
			cin >> newCarData.d.endDate;
			if (!isValidDate(newCarData.d.endDate)) {
				cout << "Invalid end date format.\n";
			}
			else if (!isDateAfterCarYear(newCarData.year, newCarData.d.endDate)) {
				cout << "End date must be after the year of the car.\n";
			}
			else if (!isEndDateAfterStartDate(newCarData.d.startDate, newCarData.d.endDate)) {
				cout << "End date must be after start date.\n";
			}
		} while (!isValidDate(newCarData.d.endDate) || !isDateAfterCarYear(newCarData.year, newCarData.d.endDate) || !isEndDateAfterStartDate(newCarData.d.startDate, newCarData.d.endDate));

	} while (!isEndDateAfterStartDate(newCarData.d.startDate, newCarData.d.endDate));

	// Write modified car data to the file
	file << newCarData.plateNumber << " " << newCarData.brand << " " << newCarData.model << " "
		<< newCarData.year << " " << newCarData.color << " " << newCarData.pricePerDay << " "
		<< newCarData.d.startDate << " " << newCarData.d.endDate << endl;

	// Close the file
	file.close();

	cout << "Car data successfully modified." << endl;
}
void displayCarInfo(const car& car3) {
	cout << "Plate Number: " << car3.plateNumber << endl;
	cout << "Start Date: " << car3.d.startDate << endl;
	cout << "End Date: " << car3.d.endDate << endl;
	cout << endl;
}
void displayCarList(const string& filename) {
	ifstream inFile(filename);
	if (!inFile) {
		cout << "Unable to open file: " << filename << endl;
		return;
	}

	car car2;
	{while (inFile >> car2.plateNumber >> car2.brand >> car2.model >> car2.year
		>> car2.color >> car2.pricePerDay >> car2.d.startDate >> car2.d.endDate)
		displayCarInfo(car2);
	}

	inFile.close();
}

// Function to check if a car is available for rental
bool checkAvailability(const string& plateNumber, const string& startDate, const string& endDate) {
	// Open the cars.txt file for reading
	ifstream inFile("cars.txt");
	if (!inFile) {
		cout << "Unable to open cars.txt file." << endl;
		return false;
	}

	// Search for the car with the given plate number
	car car1;
	while (inFile >> car1.plateNumber >> car1.brand >> car1.model >> car1.year
		>> car1.color >> car1.pricePerDay >> car1.d.startDate >> car1.d.endDate) {
		if (car1.plateNumber == plateNumber) {
			// Convert start date and end date to tm structures for comparison
			tm tm_start = {}, tm_end = {};
			stringstream ss_start(startDate);
			stringstream ss_end(endDate);
			ss_start >> get_time(&tm_start, "%d-%m-%Y");
			ss_end >> get_time(&tm_end, "%d-%m-%Y");

			// Check for invalid date formats
			if (ss_start.fail() || ss_end.fail()) {
				cout << "Invalid date format for start or end date." << endl;
				return false;
			}

			// Convert tm structures to time_t for comparison
			time_t start_time = mktime(&tm_start);
			time_t end_time = mktime(&tm_end);

			// Convert start date and end date of the car to time_t structures for comparison
			tm car_tm_start = {}, car_tm_end = {};
			stringstream ss_car_start(car1.d.startDate);
			stringstream ss_car_end(car1.d.endDate);
			ss_car_start >> get_time(&car_tm_start, "%d-%m-%Y");
			ss_car_end >> get_time(&car_tm_end, "%d-%m-%Y");

			// Check for invalid car date formats
			if (ss_car_start.fail() || ss_car_end.fail()) {
				cout << "Invalid date format for car availability." << endl;
				return false;
			}

			time_t car_start_time = mktime(&car_tm_start);
			time_t car_end_time = mktime(&car_tm_end);

			// Check if the specified start date and end date overlap with the car's availability period
			if (start_time >= car_start_time && end_time <= car_end_time) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	inFile.close();

	// If the car is not found, return false
	return false;
}
bool isPlateNumberAlreadyRentedByUser(int userID, const string& plateNumber, const string& filename) {
	ifstream inFile(filename);
	if (!inFile.is_open()) {
		cout << "Unable to open " << filename << " file." << endl;
		return false;
	}

	string fileUserID, filePlateNumber, startDate, endDate;
	while (inFile >> fileUserID >> filePlateNumber >> startDate >> endDate) {
		if (stoi(fileUserID) == userID && filePlateNumber == plateNumber) {
			inFile.close();
			return true; // Reservation already exists for the given user ID and plate number
		}
	}

	inFile.close();
	return false; // No existing reservation found for the given user ID and plate number
}

void storeRentalInfo(int userID, client& clientObj, const string& plateNumber, const string& startDate, const string& endDate) {
	if (!checkAvailability(plateNumber, startDate, endDate)) {
		cout << "Car with plate number " << plateNumber << " is not available for rental." << endl;
		return;
	}

	if (isPlateNumberAlreadyRentedByUser(userID, plateNumber, "rental_cars.txt")) {
		cout << "You have already rented a car with plate number " << plateNumber << "." << endl;
		return;
	}


	ofstream outFile("rental_cars.txt", ios::app);
	if (!outFile) {
		cout << "Unable to open rental_cars.txt file." << endl;
		return;
	}

	outFile << userID << " " << plateNumber << " " << startDate << " " << endDate << endl;
	clientObj.c->plateNumber = plateNumber;
	clientObj.c->d.startDate = startDate;
	clientObj.c->d.endDate = endDate;
	clientObj.c->isAvailable = false;


	outFile.close();

	cout << "Rental information stored successfully." << endl;
}
void displayRentedCars(const string& filename, int userID) {
	ifstream inFile(filename);
	if (!inFile.is_open()) {
		cout << "Unable to open " << filename << " file." << endl;
		return;
	}

	string fileUserID, plateNumber, startDate, endDate;
	cout << "Your rented cars:" << endl;
	while (inFile >> fileUserID >> plateNumber >> startDate >> endDate) {
		if (stoi(fileUserID) == userID) {
			cout << "Plate Number: " << plateNumber << " | Start Date: " << startDate << " | End Date: " << endDate << endl;
		}
	}
	inFile.close();
}

bool removeRentalCarByPlateNumber(const string& plateNumber, const string& filename, client& client) {
	fstream file(filename, ios::in | ios::out);
	if (!file) {
		cout<< "Error opening " << filename << " file for reading and writing." << endl;
		return false;
	}

	stringstream modifiedContent;
	bool carRemoved = false;
	string line;

	while (getline(file, line)) {
		// Extract plate number from the line
		size_t pos = line.find(plateNumber);
		if (pos != string::npos) {
			// Check if the plate number matches exactly
			size_t endPos = pos + plateNumber.size();
			if (line.substr(pos, plateNumber.size()) == plateNumber && (endPos == line.size() || line[endPos] == ' ')) {
				carRemoved = true;
				continue; // Skip writing this line to the modified content
			}
		}
		modifiedContent << line << endl;
	}

	file.close();

	if (!carRemoved) {
		cerr << "Car with plate number " << plateNumber << " not found in the rental records." << endl;
		return false;
	}

	// Reopen the file in write mode to overwrite the original file
	file.open(filename, ios::out | ios::trunc);
	if (!file) {
		cerr << "Error opening " << filename << " file for writing." << endl;
		return false;
	}

	// Write the modified content from the stringstream to the file
	file << modifiedContent.str();
	file.close();
	client.c->plateNumber = " ";
	client.c->d.startDate = " ";
	client.c->d.endDate = " ";
	client.c->isAvailable = false;
	return true;
}

void cancelReservation(const string& filename, int userID, client& client) {
	displayRentedCars(filename, userID);

	string plateNumber;
	cout << "Enter the plate number of the car you want to cancel reservation for: ";
	cin >> plateNumber;

	ifstream inFile(filename);
	if (!inFile.is_open()) {
		cerr << "Unable to open " << filename << " file." << endl;
		return;
	}

	bool plateNumberFound = false;
	string fileUserID, filePlateNumber, startDate, endDate;
	cout << "Reservations for car with plate number " << plateNumber << ":" << endl;
	while (inFile >> fileUserID >> filePlateNumber >> startDate >> endDate) {
		if (filePlateNumber == plateNumber && stoi(fileUserID) == userID) {
			cout << "UserID: " << fileUserID << " | Start Date: " << startDate << " | End Date: " << endDate << endl;
			plateNumberFound = true;
		}
	}
	inFile.close();

	if (!plateNumberFound) {
		cout << "No reservations found for car with plate number " << plateNumber << "." << endl;
		return;
	}

	if (removeRentalCarByPlateNumber(plateNumber, filename, client)) {
		cout << "Reservation for car with plate number " << plateNumber << " canceled successfully." << endl;
	}
	else {
		cout << "Failed to cancel reservation for car with plate number " << plateNumber << "." << endl;
	}
}
bool removeRentalCarByPlateNumberAndUserID(const string& plateNumber, int userID, const string& filename, client& client) {
	fstream file(filename, ios::in | ios::out);
	if (!file) {
		cout << "Error opening " << filename << " file for reading and writing." << endl;
		return false;
	}

	stringstream modifiedContent;
	bool carRemoved = false;
	string line;

	while (getline(file, line)) {
		// Extract plate number and user ID from the line
		stringstream ss(line);
		string fileUserID, filePlateNumber;
		ss >> fileUserID >> filePlateNumber;

		// Check if the plate number and user ID match
		if (filePlateNumber == plateNumber && stoi(fileUserID) == userID) {
			carRemoved = true;
			continue; // Skip writing this line to the modified content
		}

		// Write other lines to the modified content
		modifiedContent << line << endl;
	}

	file.close();

	if (!carRemoved) {
		cerr << "Car with plate number " << plateNumber << " rented by user " << userID << " not found in the rental records." << endl;
		return false;
	}

	// Reopen the file in write mode to overwrite the original file
	file.open(filename, ios::out | ios::trunc);
	if (!file) {
		cerr << "Error opening " << filename << " file for writing." << endl;
		return false;
	}

	// Write the modified content from the stringstream to the file
	file << modifiedContent.str();
	file.close();

	// Reset client data
	client.c->plateNumber = " ";
	client.c->d.startDate = " ";
	client.c->d.endDate = " ";
	client.c->isAvailable = false;

	return true;
}

void modifyRentalCarDate(int userID, client& client) {
	string filename = "rental_cars.txt"; // Assuming this is the filename used
	string plateNumber, newStartDate, newEndDate;
	cout << "Enter the plate number of the car you want to modify: ";
	cin >> plateNumber;
	do {
		cout << "Enter the new start date (DD-MM-YYYY): ";
		cin >> newStartDate;
	} while (!isValidDate(newStartDate));
	do {
		cout << "Enter the new end date (DD-MM-YYYY): ";
		cin >> newEndDate;
	} while (!isValidDate(newEndDate));

	if (!isEndDateAfterStartDate(newStartDate, newEndDate)) {
		cerr << "Invalid date(s) entered." << endl;
		return;
	}

	// Read existing reservations from the file
	ifstream inFile(filename);
	if (!inFile.is_open()) {
		cerr << "Unable to open " << filename << " file." << endl;
		return;
	}

	string unmodifiedData;
	string line;
	while (getline(inFile, line)) {
		unmodifiedData += line + "\n";
	}
	inFile.close();

	// Remove the existing reservation
	if (!removeRentalCarByPlateNumberAndUserID(plateNumber, userID, filename, client)) {
		cout << "Failed to remove existing reservation." << endl;
		return;
	}

	// Check if there is availability for the new dates
	if (!checkAvailability(plateNumber, newStartDate, newEndDate)) {
		cout << "Car is not available for the specified dates." << endl;
		// Write back the unmodified data
		ofstream outFile(filename);
		if (!outFile.is_open()) {
			cerr << "Error opening " << filename << " file for writing." << endl;
			return;
		}
		outFile << unmodifiedData;
		outFile.close();
		return;
	}
	// Append the new reservation to the file
	ofstream outFile(filename, ios::app);
	if (!outFile.is_open()) {
		cout << "Error opening " << filename << " file for appending." << endl;
		return;
	}

	outFile << userID << " " << plateNumber << " " << newStartDate << " " << newEndDate << endl;
	outFile.close();

	cout << "Rental car date modified successfully." << endl;
}
bool compareCarPrices(const car& car1, const car& car2) {
	return car1.pricePerDay < car2.pricePerDay;
}

void storeRentedCarsInPDF() {
	// Open input files
	ifstream carsFile("cars.txt");
	ofstream htmlFile("rented_cars.html");

	// Check if input files are opened successfully
	if (!carsFile || !htmlFile) {
		cout << "Error: Unable to open files." << endl;
		return;
	}

	// Open rental file to read additional data if needed
	ifstream rentalFile("rental_cars.txt");
	// You may need to process additional rental data from rentalFile

	// Initialize HTML file with table headers
	htmlFile << "<!DOCTYPE html><html><head><title>Rented Cars</title><style>table {border-collapse: collapse; width: 100%;} th, td {border: 1px solid #dddddd; text-align: left; padding: 8px;} th {background-color: #f2f2f2;}</style></head><body><h1>Rented Cars Information</h1><table><tr><th>Plate Number</th><th>Price Per Day</th></tr>";

	// Read car data from carsFile and store it in cars array
	car cars[100]; // Assuming a maximum of 100 cars, adjust as needed
	int numCars = 0;
	string plateNumber;
	double pricePerDay;
    string brand, model, color, startDate, endDate;
	int year;
	while (carsFile >> plateNumber >> brand >> model >> year >> color >> pricePerDay >> startDate >> endDate) {
		cars[numCars].plateNumber = plateNumber;
		cars[numCars].pricePerDay = pricePerDay;
		numCars++;
	}

	// Close carsFile after reading
	carsFile.close();

	// Sort cars by price in ascending order
	sort(cars, cars + numCars, compareCarPrices);

	// Append car data to the HTML file
	for (int i = 0; i < numCars; ++i) {
		htmlFile << "<tr><td>" << cars[i].plateNumber << "</td><td>" << cars[i].pricePerDay << "</td></tr>";
	}

	// Close HTML file after writing
	htmlFile << "</table></body></html>";
	htmlFile.close();

	// Convert HTML to PDF using wkhtmltopdf
	string command = "wkhtmltopdf rented_cars.html rented_cars.pdf";
	cout << "Conversion command: " << command << endl;

	// Execute the command
	int returnCode = std::system(command.c_str());
	if (returnCode != 0) {
		cout << "Error: Conversion to PDF failed with return code " << returnCode << endl;
		return;
	}

	cout << "Rented cars information stored in rented_cars.pdf successfully." << endl;
}
vector<unsigned char> sha256(const string& data) {
	vector<unsigned char> hash(SHA256_DIGEST_LENGTH);
	SHA256(reinterpret_cast<const unsigned char*>(data.c_str()), data.length(), hash.data());
	return hash;
}

int main()
{
	client user;
	char answer, answer1, answer2, answer4, answer6;
	int answer3, answer5;
	string plateNumber, plateNumber1, plateNumber2, plateNumber3, password1, password2;
	fstream countFile("users.txt");
	int userCount = 0; // Initialize userCount to 0
	int tempUserID; // Temporary variable to store user ID from file
	while (countFile >> tempUserID) {
		userCount++; // Increment userCount for each existing user ID in the file
		// Read and discard the rest of the user information in the line
		countFile.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	countFile.close();

	// Increment userCount by 1 for the new user
	userCount++;
	countFile.close();
	fstream inFile("users.txt", ios::in | ios::out);
	inFile.open("users.txt");
	cout << "Welcome to MJ's rental car services!" << endl;
	cout << endl;
	do {
		cout << "do you already have an account? (Answer with 'Y' for yes and 'N' for no.)" << endl;
		cin >> answer;
	} while (answer != 'Y' && answer != 'y' && answer != 'N' && answer != 'n');
	if (answer == 'Y' || answer == 'y')
	{
		do {
			cout << "Enter the user ID: " << endl;
			cin >> user.usertID;
		} while (isdigit(user.usertID) == true);
		do {
			cout << "enter the password: " << endl;
			cin >> password2;
		} while (isValidPassword(password2.c_str()) == false);
		vector<unsigned char> hash = sha256(password2);
		stringstream hashedPassword;
		for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
			hashedPassword << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
		}

		user.password = hashedPassword.str();

		int foundID = findUserByIDAndPassword(user.usertID, user.password);
		if (foundID != -1) {
			cout << "User found. You are an administrator." << endl;
		}
		else {
			cout << "User not found or password incorrect. You are a client." << endl;
		}
	}
	if (answer == 'N' || answer == 'n')
	{
		user.usertID = userCount;
		ofstream inFile("users.txt", ios::app);
		while (isValidName(user.firstName.c_str()) == false)
		{
			cout << "enter the first name: " << endl;
			cin >> user.firstName;
		}
		while (isValidName(user.lastName.c_str()) == false) {
			cout << "enter the last name: " << endl;
			cin >> user.lastName;
		}
		do {
			cout << "enter a valid password (its length must be at least 8, while containing numbers, letters and special characters." << endl;
			cin >> password1;
		} while (isValidPassword(password1.c_str()) == false);
		user.password = (password1);
		do {
			cout << "enter the email address: " << endl;
			cin >> user.email;
		} while (isValidEmail(user.email) == false);
		do {
			cout << "enter the phone number: " << endl;
			cin >> user.phone;
		} while (isValidPhoneNumber(user.phone) == false);
		ofstream inFile1("users.txt", ios::app);
		vector<unsigned char> hash = sha256(password1);
		inFile1 << user.usertID << " " << user.firstName << " " << user.lastName << " ";
		for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
			inFile1 << hex << setw(2) << setfill('0') << static_cast<int>(hash[i]);
		}
		inFile1 << " " << user.email << " " << user.phone << endl;
		inFile1.close();
	}

	if (answer == 'y' || answer == 'Y') {
		car newCar;
		do {
			do {
				cout << "What do you want to do?(answer with 1, 2 or 3)" << endl;
				cout << "1-Add a car" << endl;
				cout << "2-Remove a car" << endl;
				cout << "3-Modify a car" << endl;
				cin >> answer3;
			} while (answer3 != 1 && answer3 != 2 && answer3 != 3);
			if (answer3 == 1) {
				cout << "Enter car details:" << endl;
				cout << "Plate Number: ";
				cin >> newCar.plateNumber;
				do {
					cout << "Brand: ";
					cin >> newCar.brand;
				} while (isValidName(newCar.brand.c_str()) == false);
				cout << "Model: ";
				cin >> newCar.model;
				do {
					cout << "Year: ";
					cin >> newCar.year;
				} while (isValidYear(newCar.year) == false);
				do {
					cout << "Color: ";
					cin >> newCar.color;
				} while (isValidName(newCar.color.c_str()) == false);
				do {
					cout << "Price Per Day: ";
					cin >> newCar.pricePerDay;
				} while (newCar.pricePerDay <= 0);
				cout << "Enter a date in the format dd - mm - yyyy:" << endl;
				do {
					// Prompt user for start date until it's valid
					do {
						cout << "Start Date (dd-mm-yyyy): ";
						cin >> newCar.d.startDate;
						if (!isValidDate(newCar.d.startDate)) {
							cout << "Invalid start date format.\n";
						}
						else if (!isDateAfterCarYear(newCar.year, newCar.d.startDate)) {
							cout << "Start date must be after the year of the car.\n";
						}
					} while (!isValidDate(newCar.d.startDate) || !isDateAfterCarYear(newCar.year, newCar.d.startDate));

					// Prompt user for end date until it's valid and after the start date
					do {
						cout << "End Date (dd-mm-yyyy): ";
						cin >> newCar.d.endDate;
						if (!isValidDate(newCar.d.endDate)) {
							cout << "Invalid end date format.\n";
						}
						else if (!isDateAfterCarYear(newCar.year, newCar.d.endDate)) {
							cout << "End date must be after the year of the car.\n";
						}
						else if (!isEndDateAfterStartDate(newCar.d.startDate, newCar.d.endDate)) {
							cout << "End date must be after start date.\n";
						}
					} while (!isValidDate(newCar.d.endDate) || !isDateAfterCarYear(newCar.year, newCar.d.endDate) || !isEndDateAfterStartDate(newCar.d.startDate, newCar.d.endDate));

				} while (!isEndDateAfterStartDate(newCar.d.startDate, newCar.d.endDate));
				if (doesCarExist(newCar.plateNumber) == true)
					cout << "The car already exists." << endl;
				else {
					ofstream outFile1("cars.txt", ios::app);
					outFile1 << newCar.plateNumber << " " << newCar.brand << " " << newCar.model << " "
						<< newCar.year << " " << newCar.color << " " << newCar.pricePerDay << "  "
						<< newCar.d.startDate << " " << newCar.d.endDate << endl;
					outFile1.close();
				}
			}
			fstream outFile1("cars.txt", ios::in);
			outFile1.open("cars.txt");
			if (answer3 == 2) {
				cout << "Enter the plate number of the car to remove: ";
				cin >> plateNumber;
				if (removeCar(plateNumber)) {
					cout << "Car removed successfully." << endl;
				}
			}
			if (answer3 == 3) {
				modifyCar();
			}
			cout << "Do you want to do anything else?(Answer with 'Y' for yes and 'N' for no.)" << endl;
			cin >> answer2;
		} while (answer2 != 'N' && answer2 != 'n');
	}
	inFile.close();
	if (answer == 'n' || answer == 'N')
	{
		client clientObj;
		cout << "Welcome to the car rental system!" << endl;
		cout << endl;
		do {
			do {
				cout << "What do you want to do?(answer with 1, 2 or 3)" << endl;
				cout << "1-Rent a car" << endl;
				cout << "2-Cancel a rental car reservation" << endl;
				cout << "3-Modify or change the rented date of you rented car" << endl;
				cin >> answer5;
			} while (answer5 != 1 && answer5 != 2 && answer5 != 3);
			if (answer5 == 1) {
				displayCarList("cars.txt");
				cout << endl;
				cout << "Enter the plate number of the car you want to rent: " << endl;
				cin >> plateNumber2;
				string startDate, endDate;
				do {
					cout << "Enter the start date (dd-mm-yyyy): " << endl;
					cin >> startDate;
				} while (!isValidDate(startDate));
				do {
					cout << "Enter the end date (dd-mm-yyyy): " << endl;
					cin >> endDate;
				} while (!isValidDate(endDate));
				clientObj.usertID = user.usertID; // Assuming user.usertID contains the user's ID
				clientObj.c = new car();
				if (checkAvailability(plateNumber2, startDate, endDate)) {
					cout << "Car with plate number " << plateNumber << " is available from " << startDate << " to " << endDate << "." << endl;
					// Proceed with rental process if needed
				}
				else {
					cout << "Car with plate number " << plateNumber << " is not available from " << startDate << " to " << endDate << "." << endl;
					// Handle the case when the car is not available
				}
				storeRentalInfo(user.usertID, clientObj, plateNumber2, startDate, endDate);
			}
			if (answer5 == 2) {
				cancelReservation("rental_cars.txt", user.usertID, clientObj);
			}
			if (answer5 == 3) {
				displayRentedCars("rental_cars.txt", user.usertID);
				modifyRentalCarDate(user.usertID, clientObj);
			}
			cout << "Do you want to do anything else?(Answer with 'Y' for yes and 'N' for no.)" << endl;
			cin >> answer6;
		} while (answer6 != 'N' && answer6 != 'n');
		storeRentedCarsInPDF();
	}
	return 0;
}
