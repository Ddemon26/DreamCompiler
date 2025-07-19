# Classes & Structs (v1.1.12)

Dream supports object-oriented programming through classes and value-type programming through structs. These features enable encapsulation, data organization, and object-oriented design patterns for professional software development.

## Structs

Structs are value types that group related data together. They are ideal for representing simple data structures and are passed by value.

### Basic Struct Declaration

```dream
struct Point {
    int x;
    int y;
}

struct Rectangle {
    float width;
    float height;
    string color;
}

struct Person {
    string name;
    int age;
    bool isEmployed;
}
```

### Struct Usage

```dream
// Creating and using structs
func void demonstrateStructs() {
    // Declare struct variables
    Point origin;
    Point center;
    Rectangle room;
    Person employee;
    
    // Initialize struct fields
    origin.x = 0;
    origin.y = 0;
    
    center.x = 50;
    center.y = 50;
    
    room.width = 12.5;
    room.height = 8.0;
    room.color = "blue";
    
    employee.name = "John Doe";
    employee.age = 30;
    employee.isEmployed = true;
    
    // Use struct data
    Console.WriteLine("Origin: (" + origin.x + ", " + origin.y + ")");
    Console.WriteLine("Room: " + room.width + "x" + room.height + " " + room.color);
    Console.WriteLine("Employee: " + employee.name + ", age " + employee.age);
}
```

### Structs with Functions

```dream
// Functions that work with structs
func float calculateDistance(Point p1, Point p2) {
    int dx = p2.x - p1.x;
    int dy = p2.y - p1.y;
    return sqrt(dx * dx + dy * dy);  // Conceptual sqrt function
}

func float calculateArea(Rectangle rect) {
    return rect.width * rect.height;
}

func bool isAdult(Person person) {
    return person.age >= 18;
}

func Point movePoint(Point p, int deltaX, int deltaY) {
    Point newPoint;
    newPoint.x = p.x + deltaX;
    newPoint.y = p.y + deltaY;
    return newPoint;
}

// Usage examples
func void structFunctionDemo() {
    Point start;
    start.x = 10;
    start.y = 20;
    
    Point end;
    end.x = 30;
    end.y = 40;
    
    float distance = calculateDistance(start, end);
    Console.WriteLine("Distance: " + distance);
    
    Point moved = movePoint(start, 5, -3);
    Console.WriteLine("Moved point: (" + moved.x + ", " + moved.y + ")");
}
```

### Complex Struct Examples

```dream
struct Vector2D {
    float x;
    float y;
}

struct Color {
    int red;
    int green;
    int blue;
    int alpha;
}

struct GameEntity {
    Vector2D position;
    Vector2D velocity;
    Color tint;
    bool isActive;
    int health;
}

struct BankAccount {
    string accountNumber;
    string ownerName;
    float balance;
    bool isActive;
    int transactionCount;
}

// Working with complex structs
func void complexStructDemo() {
    // Game entity example
    GameEntity player;
    player.position.x = 100.0;
    player.position.y = 200.0;
    player.velocity.x = 5.0;
    player.velocity.y = 0.0;
    player.tint.red = 255;
    player.tint.green = 255;
    player.tint.blue = 255;
    player.tint.alpha = 255;
    player.isActive = true;
    player.health = 100;
    
    // Bank account example
    BankAccount account;
    account.accountNumber = "ACC001";
    account.ownerName = "Jane Smith";
    account.balance = 1500.75;
    account.isActive = true;
    account.transactionCount = 0;
    
    Console.WriteLine("Player at (" + player.position.x + ", " + player.position.y + ")");
    Console.WriteLine("Account " + account.accountNumber + " balance: $" + account.balance);
}
```

## Classes

Classes are reference types that support encapsulation, inheritance, and more advanced object-oriented features.

### Basic Class Declaration

```dream
class Vehicle {
    string make;
    string model;
    int year;
    float mileage;
    bool isRunning;
}

class Student {
    string name;
    int studentId;
    float gpa;
    string major;
    int creditHours;
}

class Rectangle {
    float width;
    float height;
    string color;
    bool isFilled;
}
```

### Class Usage

```dream
// Creating and using classes
func void demonstrateClasses() {
    // Create class instances using 'new'
    Vehicle car = new Vehicle();
    Student alice = new Student();
    Rectangle shape = new Rectangle();
    
    // Initialize class fields
    car.make = "Toyota";
    car.model = "Camry";
    car.year = 2022;
    car.mileage = 15000.5;
    car.isRunning = false;
    
    alice.name = "Alice Johnson";
    alice.studentId = 12345;
    alice.gpa = 3.75;
    alice.major = "Computer Science";
    alice.creditHours = 15;
    
    shape.width = 10.0;
    shape.height = 5.0;
    shape.color = "red";
    shape.isFilled = true;
    
    // Use class data
    Console.WriteLine("Car: " + car.year + " " + car.make + " " + car.model);
    Console.WriteLine("Student: " + alice.name + " (ID: " + alice.studentId + ")");
    Console.WriteLine("Rectangle: " + shape.width + "x" + shape.height + " " + shape.color);
}
```

### Classes with Methods (Conceptual)

While method syntax is being finalized, here's how class functionality might work:

```dream
// Note: Method syntax may vary in final implementation
class Calculator {
    float lastResult;
    bool hasError;
    
    // Methods would be declared within the class
}

// For now, use functions that operate on class instances
func float add(Calculator calc, float a, float b) {
    float result = a + b;
    calc.lastResult = result;
    calc.hasError = false;
    return result;
}

func float divide(Calculator calc, float a, float b) {
    if (b == 0.0) {
        calc.hasError = true;
        return 0.0;
    }
    
    float result = a / b;
    calc.lastResult = result;
    calc.hasError = false;
    return result;
}

func void clearCalculator(Calculator calc) {
    calc.lastResult = 0.0;
    calc.hasError = false;
}

// Usage
func void calculatorDemo() {
    Calculator calc = new Calculator();
    clearCalculator(calc);
    
    float sum = add(calc, 10.0, 5.0);
    Console.WriteLine("10 + 5 = " + sum);
    
    float quotient = divide(calc, 10.0, 0.0);
    if (calc.hasError) {
        Console.WriteLine("Error: Division by zero");
    }
}
```

## Advanced Examples

### Data Management Classes

```dream
class Employee {
    int employeeId;
    string firstName;
    string lastName;
    string department;
    float salary;
    bool isActive;
    int yearsOfService;
}

class Project {
    string projectId;
    string title;
    string description;
    float budget;
    bool isCompleted;
    int teamSize;
}

// Employee management functions
func string getFullName(Employee emp) {
    return emp.firstName + " " + emp.lastName;
}

func float calculateAnnualSalary(Employee emp) {
    return emp.salary * 12.0;
}

func bool isEligibleForPromotion(Employee emp) {
    return emp.yearsOfService >= 2 && emp.isActive;
}

func void promoteEmployee(Employee emp, float salaryIncrease) {
    emp.salary += salaryIncrease;
    Console.WriteLine(getFullName(emp) + " promoted with $" + salaryIncrease + " increase");
}

// Project management functions
func float calculateCostPerMember(Project proj) {
    if (proj.teamSize == 0) return 0.0;
    return proj.budget / proj.teamSize;
}

func void completeProject(Project proj) {
    proj.isCompleted = true;
    Console.WriteLine("Project '" + proj.title + "' marked as completed");
}
```

### Game Development Structures

```dream
struct Vector3 {
    float x;
    float y;
    float z;
}

class GameObject {
    Vector3 position;
    Vector3 rotation;
    Vector3 scale;
    string name;
    bool isActive;
    int layer;
}

class Player {
    GameObject gameObject;
    int health;
    int maxHealth;
    float speed;
    string playerName;
    int score;
    bool canMove;
}

// Game object functions
func void setPosition(GameObject obj, float x, float y, float z) {
    obj.position.x = x;
    obj.position.y = y;
    obj.position.z = z;
}

func void moveObject(GameObject obj, float deltaX, float deltaY, float deltaZ) {
    obj.position.x += deltaX;
    obj.position.y += deltaY;
    obj.position.z += deltaZ;
}

func float getDistanceTo(GameObject obj1, GameObject obj2) {
    float dx = obj2.position.x - obj1.position.x;
    float dy = obj2.position.y - obj1.position.y;
    float dz = obj2.position.z - obj1.position.z;
    return sqrt(dx*dx + dy*dy + dz*dz);
}

// Player functions
func void takeDamage(Player player, int damage) {
    player.health -= damage;
    if (player.health < 0) {
        player.health = 0;
    }
    
    Console.WriteLine(player.playerName + " took " + damage + " damage");
    
    if (player.health == 0) {
        Console.WriteLine(player.playerName + " has been defeated!");
        player.canMove = false;
    }
}

func void healPlayer(Player player, int healAmount) {
    player.health += healAmount;
    if (player.health > player.maxHealth) {
        player.health = player.maxHealth;
    }
    
    Console.WriteLine(player.playerName + " healed for " + healAmount + " points");
}

// Game demo
func void gameDemo() {
    Player hero = new Player();
    hero.gameObject = new GameObject();
    hero.gameObject.name = "Hero";
    hero.playerName = "Warrior";
    hero.health = 100;
    hero.maxHealth = 100;
    hero.speed = 5.0;
    hero.score = 0;
    hero.canMove = true;
    
    setPosition(hero.gameObject, 10.0, 0.0, 5.0);
    
    takeDamage(hero, 25);  // Hero takes damage
    healPlayer(hero, 10);  // Hero heals
    
    moveObject(hero.gameObject, 2.0, 0.0, 1.0);  // Move hero
}
```

## Memory Management

Classes in Dream use automatic reference counting:

```dream
func void memoryDemo() {
    // Classes are reference types
    Vehicle car1 = new Vehicle();
    car1.make = "Honda";
    car1.model = "Civic";
    
    Vehicle car2 = car1;  // car2 references the same object as car1
    car2.year = 2023;     // Modifies the same object
    
    Console.WriteLine("Car1 year: " + car1.year);  // Prints: Car1 year: 2023
    
    // Structs are value types
    Point p1;
    p1.x = 10;
    p1.y = 20;
    
    Point p2 = p1;  // p2 is a copy of p1
    p2.x = 30;      // Only modifies p2
    
    Console.WriteLine("P1 x: " + p1.x);  // Prints: P1 x: 10
    Console.WriteLine("P2 x: " + p2.x);  // Prints: P2 x: 30
}
```

## Best Practices

### Struct Design
```dream
// Keep structs simple and focused
struct Point2D {
    float x;
    float y;
}

struct Color {
    int r;
    int g;
    int b;
}

// Use structs for small, value-oriented data
struct Temperature {
    float celsius;
    bool isValid;
}

// Provide helper functions for struct operations
func Point2D addPoints(Point2D p1, Point2D p2) {
    Point2D result;
    result.x = p1.x + p2.x;
    result.y = p1.y + p2.y;
    return result;
}
```

### Class Design
```dream
// Use classes for complex entities
class DatabaseConnection {
    string connectionString;
    bool isConnected;
    int timeoutSeconds;
    string lastError;
}

class UserSession {
    string userId;
    string sessionToken;
    bool isAuthenticated;
    int expirationTime;
    string[] permissions;
}

// Group related functionality
func bool connectToDatabase(DatabaseConnection conn) {
    // Connection logic
    conn.isConnected = true;
    conn.lastError = "";
    return true;
}

func void disconnectFromDatabase(DatabaseConnection conn) {
    conn.isConnected = false;
    conn.lastError = "";
}
```

### Error Handling
```dream
func bool validateEmployee(Employee emp) {
    if (emp.firstName == "") {
        throw "Employee first name cannot be empty";
    }
    if (emp.lastName == "") {
        throw "Employee last name cannot be empty";
    }
    if (emp.salary < 0.0) {
        throw "Employee salary cannot be negative";
    }
    return true;
}

func void safeEmployeeCreation() {
    Employee emp = new Employee();
    emp.firstName = "John";
    emp.lastName = "Smith";
    emp.salary = 50000.0;
    
    try {
        validateEmployee(emp);
        Console.WriteLine("Employee created successfully");
    } catch {
        Console.WriteLine("Failed to create employee");
    }
}
```

Classes and structs in Dream provide powerful abstraction mechanisms for organizing code and data, supporting both value-oriented and object-oriented programming paradigms for professional software development.

