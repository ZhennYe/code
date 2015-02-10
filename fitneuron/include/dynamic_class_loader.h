#ifndef _DYNAMIC_CLASS_LOADER_H_
#define _DYNAMIC_CLASS_LOADER_H_



#include <stdexcept>
#include <map>
#include <string>
#include <set>
#include <list>
#include <dlfcn.h>
// dirent.h only needed for getting files in directory,
//    can get rid of it by re-writing getLibFilesInDir()
#include <dirent.h>
// fstream only needed for Factory<T>::fileExists()
#include <fstream>



//When using dlfcn to dynamically link, use library ldl
//USE_LINK_FLAG -ldl
// also use linker flags -fPIC and -rdynamic
//USE_LINK_FLAG -fPIC -rdynamic
//USE_COMPILE_FLAG -fPIC



// Use this header to dynamically load classes:
// 1. Create a base class.  e.g.:
//   class Cat { stuff };
// 2. In the header file for the base class, define a Factory as extern.  e.g.:
//   extern Factory<Cat> catFactory;
// 3. Create derived classes in separate files.  e.g.:
//   class Tabby : public Cat { stuff };
// 4. In the derived class file, register the derived class for dynamic loading
//   REGISTER_CLASS(Cat, Tabby, catFactory)
// 5. Compile the derived classes to shared object/dll files
// 6. In the main program, define the Factory as a global variable e.g.:
//   Factory<Cat> catFactory;
// 7. Where you need to use the factory to dynamically load class objects:
//   a) Define the factory as extern
//     extern Factory<Cat> catFactory;
//   b) Make the factory load the derived class libraries
//     catFactory.loadLibrariesInDirectory(catDir);
//   c) Create new cats as needed
//     std::vector<Cat*> cats;
//     cats.push_back(catFactory.createNew("Tabby"));
//   d) Clean up by deleting the elements of cats
//     for(catItr = cats.begin(); catItr != cats.end(); ++catItr)
//       delete *catItr
// Note that CreateMakefile.py can automatically make the shared objects for
// this directory by passing it "-shared dirName", e.g.
//   CreateMakefile.py doCatStuff.bin -shared cats



// list of exceptions
// Base exeption, derived from std::runtime_error
class DynamicClassLoaderException;
// exceptions derived from DynamicClassLoaderException:
class InvalidDynamicClassException;
class DuplicateDynamicClassException;
class LoadLibraryException;
class DuplicateLibraryException;
class InvalidDirectoryException;



template <class T>
class Factory
{
  public:
    ~Factory() { closeLibraries(); }
  
    // some typedefs
    typedef T* (*MakerType)(void);
    typedef std::map<std::string, MakerType, std::less<std::string> > MapType;
    
    // public member functions
    T* createNew(const std::string & dynamicName) const;
    void registerClass(const std::string & dynamicName, MakerType maker);
    void loadLibrary(const std::string & libFileName);
    void loadLibrariesInDirectory(const std::string & dirName);
    void closeLibraries(void);
    std::set<std::string> getDynamicNames(void) const {
      std::set<std::string> keys;
      for(const auto & mapObj : factoryMap) {
        keys.insert(mapObj.first);
      }
      return std::move(keys);
    }
      
  protected:
    // protected member data
    MapType factoryMap;
    std::map<std::string, void*> dynamicLibraryMap;
    std::set<std::string> libraryPaths;
  private:
    inline bool fileExists(const std::string & fileName);
    inline bool endsWith(const std::string & fullStr,
                         const std::string & endStr);
    std::list<std::string> getLibFilesInDir(const std::string & dirName);
};



#define REGISTER_CLASS(baseClass, derivedClass, factory) \
extern "C" { \
baseClass* maker() \
{ \
   return new derivedClass; \
} \
class proxy \
{ \
  public: \
    proxy() { \
      factory.registerClass(#derivedClass, maker); \
   } \
}; \
proxy p; \
}

#define REGISTER_BASE_CLASS(baseClass, factory) \
extern Factory<baseClass> factory;



////////////////////////// Factory member functions ///////////////////////////



template <class T>
void
Factory<T>::closeLibraries(void)
{
  // close the handles to the dynamic libraries we've loaded
  for(std::map<std::string, void*>::iterator libItr =
        dynamicLibraryMap.begin();
      libItr != dynamicLibraryMap.end();
      ++libItr)
    dlclose(libItr->second);
}



template <class T>
T*
Factory<T>::createNew(const std::string & dynamicName) const
{
  typename MapType::const_iterator mapItr = factoryMap.find(dynamicName);
  if(mapItr == factoryMap.end())
    // dynamicName has not been registered with the map, throw an error
    throw InvalidDynamicClassException(dynamicName);
  else
    // dynamicName is registered
    
    // get a pointer to the appropriate class, and invoke it 
    return mapItr->second ();
}



template <class T>
void
Factory<T>::registerClass(const std::string & dynamicName,
                               MakerType maker)
{
  typename MapType::const_iterator mapItr = factoryMap.find(dynamicName);
  if(mapItr == factoryMap.end())
    // dynamicName has not been registered with the map, add it
    factoryMap[dynamicName] = maker;
  else {
    // dynamicName is already registered, throw an error
    throw DuplicateDynamicClassException(dynamicName);
  }

  
}



template <class T>
void
Factory<T>::loadLibrary(const std::string & libFileName)
{
  std::map<std::string, void*>::iterator libItr =
    dynamicLibraryMap.find(libFileName);
  if(libItr == dynamicLibraryMap.end()) {
    // this is a new library
    //   -load the library
    
    // check to make sure the file name points to a real file
    if(!fileExists(libFileName))
      throw LoadLibraryException(libFileName, "file does not exist");
    
    void* libHandle = dlopen(libFileName.c_str(), RTLD_LAZY);
    if(libHandle == NULL) {
      std::string problem (dlerror());
      throw LoadLibraryException(libFileName, problem);
    }
    
    //   -store handle in map of libraries
    dynamicLibraryMap[libFileName] = libHandle;
    
  }
  else
    // this library was already loaded.  Throw an exception
    throw DuplicateLibraryException(libFileName);
}



template <class T>
void
Factory<T>::loadLibrariesInDirectory(const std::string & dirName)
{
  std::set<std::string>::iterator setItr = libraryPaths.find(dirName);
  if(setItr != libraryPaths.end())
    // libraries from this directory are already loaded, skip it quietly
    return;
  
  // add this directory to the list of directories that have been opened
  libraryPaths.insert(dirName);
  
  // loop through all library names and load them
  std::list<std::string> libFiles = getLibFilesInDir(dirName);
  std::list<std::string>::const_iterator libFileItr = libFiles.begin();
  for(; libFileItr != libFiles.end(); libFileItr++)
    loadLibrary(*libFileItr);
}



template <class T>
inline bool
Factory<T>::fileExists(const std::string & fileName)
{
  // test if file exists
  
  std::ifstream fileTest (fileName.c_str());
  if(fileTest.good()) {
    fileTest.close();
    return true;
  }
  return false;
}



template <class T>
inline bool
Factory<T>::endsWith(const std::string & fullStr, const std::string & endStr)
{
  // test if fullStr ends with endStr
  
  if(fullStr.length() > endStr.length())
    return (0 == fullStr.compare(fullStr.length() - endStr.length(),
                                 endStr.length(), endStr));
  else
    return false;
}



template <class T>
std::list<std::string>
Factory<T>::getLibFilesInDir(const std::string & dirName)
{
  std::list<std::string> libFiles;
  
  // loop through all files in directory and save names of library files
  std::list<std::string> libraryNames;
  struct dirent *entry;
  DIR* dirPtr = opendir(dirName.c_str());
  if(dirPtr == NULL)
    // Couldn't open directory, throw exception
    throw InvalidDirectoryException(dirName);
  
  entry = readdir(dirPtr);
  while(entry != NULL) {
    std::string libName (entry->d_name);
    if(endsWith(libName, ".so") || endsWith(libName, ".dll"))
      libFiles.push_back(dirName + "/" + libName);
    entry = readdir(dirPtr);
  }
  closedir(dirPtr);
  
  return libFiles;
}



//////////////////////// Exception class definitions //////////////////////////



class DynamicClassLoaderException : public std::runtime_error
{
 public:
   DynamicClassLoaderException() : std::runtime_error(
     "DynamicClassLoaderException") { }
   DynamicClassLoaderException(const std::string & str) :
     std::runtime_error(str) { }
 };
 

 
class InvalidDynamicClassException : public DynamicClassLoaderException
{
  public:
    InvalidDynamicClassException() :
      DynamicClassLoaderException("InvalidDynamicClassExeption") {}
    InvalidDynamicClassException(const std::string & className) :
      DynamicClassLoaderException(
      "Exeption loading dynamic class: unknown class \"" + className + "\"") {}
};



class DuplicateDynamicClassException : public DynamicClassLoaderException
{
  public:
    DuplicateDynamicClassException() :
      DynamicClassLoaderException("DuplicateClassExeption") {}
    DuplicateDynamicClassException(const std::string & className) :
      DynamicClassLoaderException(
      "Exeption registering dynamic class: duplicate class \"" + className) {}
};



class LoadLibraryException : public DynamicClassLoaderException
{
  public:
    LoadLibraryException() :
      DynamicClassLoaderException("LoadLibraryExeption") {}
    LoadLibraryException(const std::string & libName) :
      DynamicClassLoaderException(
        "Exception loading library \"" + libName + "\"") {}
    LoadLibraryException(const std::string & libName,
                         const std::string & problem) :
      DynamicClassLoaderException(
        "Exception loading library \"" + libName + "\": " + problem) {}
};



class DuplicateLibraryException : public DynamicClassLoaderException
{
  public:
    DuplicateLibraryException() :
      DynamicClassLoaderException("DuplicateLibraryExeption") {}
    DuplicateLibraryException(const std::string & libName) :
      DynamicClassLoaderException(
        "Exception loading library: duplicate library \"" + libName + "\"") {}
};



class InvalidDirectoryException : public DynamicClassLoaderException
{
  public:
    InvalidDirectoryException() :
      DynamicClassLoaderException("InvalidDirectoryExeption") {}
    InvalidDirectoryException(const std::string & dirName) :
      DynamicClassLoaderException(
        "Files in directory \"" + dirName + "\" could not be listed.") {}
};



#endif
