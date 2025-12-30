# Car Rental System

A comprehensive C++ car rental management system with user authentication, booking management, and PDF report generation.

## Features

### User Management
- **User Registration**: Create new accounts with secure password hashing (SHA-256)
- **User Authentication**: Login system with encrypted password verification
- **Role-Based Access**: Separate interfaces for administrators and clients
- **Input Validation**: Email, phone number, and password strength validation

### Administrator Features
- **Add Cars**: Register new vehicles with complete details
- **Remove Cars**: Delete vehicles from the inventory
- **Modify Cars**: Update existing vehicle information
- **Inventory Management**: Full control over the car fleet

### Client Features
- **Browse Cars**: View available vehicles with details
- **Rent Cars**: Book vehicles for specific date ranges
- **Cancel Reservations**: Remove existing bookings
- **Modify Bookings**: Change rental dates for reserved vehicles
- **Rental History**: View personal rental records
- **PDF Reports**: Generate sorted rental reports

### Booking System
- **Availability Checking**: Real-time availability verification
- **Date Validation**: Ensures valid booking dates
- **Conflict Prevention**: Prevents double-booking
- **Flexible Dates**: Modify reservations as needed

### Security Features
- **SHA-256 Encryption**: Secure password hashing using OpenSSL
- **Password Requirements**: Minimum 8 characters with letters, numbers, and special characters
- **User Verification**: ID and password authentication

### Data Management
- **File-Based Storage**: Persistent data using text files
- **PDF Generation**: Creates sorted car reports using wkhtmltopdf
- **Data Validation**: Comprehensive input checking

## Technical Implementation

### Data Structures
- **Structs**: Custom data structures for Date, Car, and Client entities
- **Pointers**: Dynamic memory management for car assignments
- **File I/O**: Text file-based data persistence

### Security
- **OpenSSL Integration**: SHA-256 cryptographic hashing
- **Password Validation**: Multiple security checks
- **User Authentication**: Secure login system

### External Dependencies
- **OpenSSL**: For SHA-256 password hashing
- **wkhtmltopdf**: For PDF report generation

## Installation

### Prerequisites
- C++11 or later compiler
- OpenSSL library
- wkhtmltopdf (for PDF generation)

## Usage

### Running the Program
```bash
./car_rental
```

### First Time Setup
1. Create a new account or login with existing credentials
2. Administrators can manage the car inventory
3. Clients can browse and rent available cars

### File Structure
```
car-rental-system/
├── main.cpp              # Main source code
├── users.txt             # User database (auto-created)
├── cars.txt              # Car inventory (auto-created)
├── rental_cars.txt       # Rental records (auto-created)
├── rented_cars.html      # Generated HTML report (temporary)
├── rented_cars.pdf       # Final PDF report
└── README.md             # This file
```

## Input Formats

### Date Format
```
DD-MM-YYYY
Example: 15-03-2024
```

### Password Requirements
- Minimum 8 characters
- At least one letter
- At least one number
- At least one special character

### Phone Number
- 8 digits
- Can include spaces, dashes, parentheses

### Email
- Valid email format with @ and domain

## Features in Detail

### Car Information Includes:
- Plate Number (unique identifier)
- Brand and Model
- Year of manufacture
- Color
- Price per day
- Availability dates (start and end)

### Rental Process:
1. View available cars
2. Select car by plate number
3. Choose rental dates
4. System checks availability
5. Booking confirmation

### PDF Report:
- Sorted by price (ascending)
- Includes all rental information
- Professional HTML-to-PDF conversion

## Error Handling

The system validates:
- Invalid dates and date ranges
- Duplicate reservations
- Non-existent users or cars
- Insufficient data
- Date conflicts
- Password strength
- Email and phone formats

## Security Notes

- Passwords are hashed using SHA-256
- Never stores plain-text passwords
- User authentication required for all operations
- File-based storage (consider database for production)


## Short Description for GitHub
```
C++ car rental management system with SHA-256 authentication, booking system, and PDF report generation
