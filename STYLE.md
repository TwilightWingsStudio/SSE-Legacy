# Serious Sam Evolution coding standarts
========================

If you wish to submit a pull request to Serious Sam Evolution then please take a look at the sections below about our coding and styling standards before making said pull request.

========================
## Coding standards (.h / .hpp / .cpp / .c / .es)
Following our coding standards is absolutely essential to have your pull request approved. While we may not close your pull request if it doesn't follow these coding standards, we most likely will delay merging it until compliant.

### Coding standarts - Prerequisites
- Avoid doing large commits. I prefer to have a single commit for a tiny fix/addition rather than bundled up commits.
- Always make sure all files contain the GNU GPL-2.0 license at the beginning of every file containing buildable source code.

### Coding standarts - Operators
- ALWAYS should be space between operator keyword and round braces!
- ALWAYS should be space between operator round braces and block braces.

```C
// WRONG

if(a > b){
  // Your code here.
}

// WRONG

if(a > b) {
  // Your code here.
}

// WRONG

if (a > b){
  // Your code here.
}

// WRONG

while(true) {
  // Your code here.
}

// WRONG

while (true){
  // Your code here.
}

// WRONG

while(true){
  // Your code here.
}

// WRONG

for(int i = 0; i < 10; i++){
  // Your code here.
}

// WRONG

for(int i = 0; i < 10; i++) {
  // Your code here.
}

// WRONG

for (int i = 0; i < 10; i++){
  // Your code here.
}

// RIGHT

if (a > b) {
  // Your code here.
}

// RIGHT

while (true) {
  // Your code here.
}

// RIGHT

for (int i = 0; i < 10; i++) {
  // Your code here.
}

```

### Coding standarts - Methods and Functions
- Method body braces should ALWAYS be on the separate line!

```C
// WRONG

void SomeFunc() {
  // Your code here.
}

// RIGHT

void SomeFunc()
{
  // Your code here.
}
```

### Coding standarts - Class Members
- Everytning under the public/private/protected sections should be ALWAYS moved a bit to the right.

```C
// WRONG

class CSomeClass
{
  public:
  void SomeFunc1();

  protected:
  void InternalCalc();
    
  private:
  int m_iStuff;
  bool m_bFlag;
};

// WRONG

class CSomeClass
{
public:
  void SomeFunc1();

protected:
  void InternalCalc();
    
private:
  int m_iStuff;
  bool m_bFlag;
};

// RIGHT

class CSomeClass
{
  public:
    void SomeFunc1();

  protected:
    void InternalCalc();
    
  private:
    int m_iStuff;
    bool m_bFlag;
};
```

### Coding standarts - Tabs
- NEVER use tabs! Always use 2 spaces instead of tabs.
```C
// WRONG

void SomeFunc()
{
    if (m_bSomeFlag) {
        m_iSomeVar = 3;
    }
}

// RIGHT

void SomeFunc()
{
  if (m_bSomeFlag) {
    m_iSomeVar = 3;
  }
}
```

### Coding standarts - switch members (cases) ALWAYS must have tabs.
- NEVER write switch-case construction without having tabs for every case block.
```C
// WRONG

switch (iSomeVar)
{
case 0: {
  ...
} break;

case 1: {
  ...
} break;

case 3: {
  ...
} break;

default: break;    
}

// RIGHT

switch (iSomeVar)
{
  case 0: {
    ...
  } break;

  case 1: {
    ...
  } break;

  case 3: {
    ...
  } break;

  default: break;    
}
```

========================
## Styling guidelines (.h / .hpp / .cpp / .c / .es)
These styling guidelines represent the way I prefer to have all the things styled. Ignoring these guidelines will not affect my decision on approving your pull request.

### Style - Operator without braces
- Avoid using opeators without braces.
```C
// WRONG

if (bSomeFlag) return;
  
// WRONG

if (condition)
  return;

// RIGHT

if (condition) {
  return;
}
```

### Style - Empty lines
- Avoid putting more than one empty line at any time.
```C
// WRONG

void SomeFunc(condition)
{
  if (condition) {
    ...
  }
  

  if (condition) {
    ...
  }
}

// RIGHT

void SomeFunc()
{
  if (condition) {
    ...
  }

  if (condition) {
    ...
  }
}
```
