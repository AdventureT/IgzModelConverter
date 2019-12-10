//#include <tchar.h>
//#include <stdio.h>
#include <array>
#include <thread>
#include <vector>
// some headers should be included first, to activate macro definitions for
// others
#include "../datas/endian.hpp"
#include "../datas/reflector.hpp"

#include "../datas/DirectoryScanner.hpp"
#include "../datas/Matrix33Simple.hpp"
#include "../datas/allocator_hybrid.hpp"
#include "../datas/blowfish2.h"
#include "../datas/disabler.hpp"
#include "../datas/esstring.h"
#include "../datas/fileinfo.hpp"
#include "../datas/flags.hpp"
#include "../datas/halfFloat.hpp"
#include "../datas/masterprinter.hpp"
#include "../datas/reflectorRegistry.hpp"
#include "../datas/VectorsSimd.hpp"
#include "../datas/xorenc.hpp"

// some headers should be included last, to activate all possible macro
// contitionals
#include "../datas/binreader.hpp"
#include "../datas/binwritter.hpp"

/************************************************************************/
/************************ REFLECTOR SAMPLE ******************************/
/************************************************************************/

REFLECTOR_CREATE(EnumWrap00, ENUM, 1, CLASS, E1, E2,
                 E3 = 0x7); // as enum class EnumWrap00 {};
REFLECTOR_CREATE(EnumWrap01, ENUM, 0, EnumWrap01_E1, EnumWrap01_E2,
                 EnumWrap01_E3); // as enum EnumWrap01 {};
REFLECTOR_CREATE(EnumWrap02, ENUM, 2, CLASS, 8, E4, E5,
                 E6); // as enum class EnumWrap02 : uchar {};
REFLECTOR_CREATE(EnumWrap03, ENUM, 2, CLASS, 16, E7 = 7, E8 = 16586,
                 E9 = 0x8bcd); // as enum class EnumWrap03 : ushort {};
REFLECTOR_CREATE(EnumWrap04, ENUM, 2, CLASS, 32, E10, E11,
                 E12); // as enum class EnumWrap04 : uint {};

const short int hhrt = sizeof(long double);

struct subrefl {
  int data;
  float data2;

  void
  SwapEndian() // swapper function for fbyteswapper called from reflClass class
  {
    FByteswapper(data);
    FByteswapper(data2);
  }
};

struct _ReflClassData {
  bool test1;
  char test2;
  uchar test3;
  short test4;
  ushort test5;
  int test6;
  uint test7;
  int64 test8;
  uint64 test9;
  float test10;
  double test11;
  esFlags<uchar, EnumWrap00> test12;

  EnumWrap00 test13;
  EnumWrap02 test14;
  EnumWrap03 test19;
  EnumWrap04 test20;

  Vector test15;
  Vector2 test16;
  Vector4A16 test17;

  int test21[4];

  EnumWrap03 test23[2];

  subrefl test18;

  void
  SwapEndian() // swapper function for fbyteswapper called from bincore classes
  {
    FByteswapper(test4);
    FByteswapper(test5);
    FByteswapper(test6);
    FByteswapper(test7);
    FByteswapper(test8);
    FByteswapper(test9);
    FByteswapper(test10);
    FByteswapper(test11);
    FByteswapper(test12);
    FByteswapper(test13);
    FByteswapper(test14);
    FByteswapper(test15);
    FByteswapper(test16);
    FByteswapper(test17);
    FByteswapper(test18);
  }

  _ReflClassData() = default;
};

struct reflClass : Reflector, _ReflClassData {
  DECLARE_REFLECTOR;

  std::string test22;

  reflClass() = default;
};

REFLECTOR_CREATE(reflClass, 0, test1, test2, test3, test4, test5, test6, test7,
                 test8, test9, test10, test11, test12, test13, test14, test15,
                 test16, test17, test18.data, test18.data2, test19, test20,
                 test21, test22, test23)

REGISTER_ENUMS(EnumWrap00, EnumWrap02, EnumWrap03, EnumWrap04)

// uses reflector, esstring as convertor, masterprinter
reflClass ReflectorTest() {
  RegisterLocalEnums();
  reflClass test = {};

  // Previews of reflType entries
  const reflType *types = reflClass::__rfPtrStatic->types;
  const reflType *types2 = reflClass::__rfPtrStatic->types + 1;
  const reflType *types3 = reflClass::__rfPtrStatic->types + 2;
  const reflType *types4 = reflClass::__rfPtrStatic->types + 3;
  const reflType *types5 = reflClass::__rfPtrStatic->types + 4;
  const reflType *types6 = reflClass::__rfPtrStatic->types + 5;
  const reflType *types7 = reflClass::__rfPtrStatic->types + 6;
  const reflType *types8 = reflClass::__rfPtrStatic->types + 7;
  const reflType *types9 = reflClass::__rfPtrStatic->types + 8;
  const reflType *types10 = reflClass::__rfPtrStatic->types + 9;
  const reflType *types11 = reflClass::__rfPtrStatic->types + 10;
  const reflType *types12 = reflClass::__rfPtrStatic->types + 11;
  const reflType *types13 = reflClass::__rfPtrStatic->types + 12;
  const reflType *types14 = reflClass::__rfPtrStatic->types + 13;
  const reflType *types15 = reflClass::__rfPtrStatic->types + 14;
  const reflType *types16 = reflClass::__rfPtrStatic->types + 15;
  const reflType *types17 = reflClass::__rfPtrStatic->types + 16;

  // setting values
  test.SetReflectedValue("test1", "true");
  test.SetReflectedValue("test2", "-107");
  test.SetReflectedValue("test3", "157");
  test.SetReflectedValue("test4", "-15798");
  test.SetReflectedValue("test5", "15785");

  test.SetReflectedValue("test6", "-15798078");
  test.SetReflectedValue("test7", "4294967290");

  test.SetReflectedValue("test8", "-9223372036854775800");
  test.SetReflectedValue("test9", "12233720368547758004");

  test.SetReflectedValue("test10", "1.59178762832786378278276817837");
  test.SetReflectedValue("test11", "1.5978317365453265638626274637212");

  test.SetReflectedValue("test12", "E1 | E2 | E3");

  test.SetReflectedValue("test13", "E2");

  test.SetReflectedValue("test14", "E5");

  test.SetReflectedValue("test15", "[1.5, 2.8, 5.4]");
  test.SetReflectedValue("test16", "[5.81, 2.1]");
  test.SetReflectedValue("test17", "[1.15, 2.631, 4.1, 15161.196541]");

  test.SetReflectedValue("test18.data", "182");
  test.SetReflectedValue("test18.data2", "182.8744");

  test.SetReflectedValue("test19", "E9");
  test.SetReflectedValue("test20", "E11");

  test.SetReflectedValue("test21", "{ 5, 4, 3, 2}");
  test.SetReflectedValue("test22", "test string");

  test.SetReflectedValue("test23", "{ E9, E8 }");

  // Gets name and value pair, names are working only if
  // DECLARE_REFLECTOR_WNAMES is used
  Reflector::KVPair test1val = test.GetReflectedPair(16);

  // test.ToXML(_T("testfile.xml"));

  // test = {};

  // test.FromXML(_T("testfile.xml"));

  printline("Printing all reflected values")

      for (int r = 0; r < test.GetNumReflectedValues(); r++) {
    printer << esString(test.GetReflectedValue(r)) >> 1;
  }

  return test;
}

/************************************************************************/
/******** BINWRITTER, BINREADER, TFILEINFO, MASTERPRINTER SAMPLE ********/
/************************************************************************/
void FileIO(const TCHAR *folderPath) {
  TSTRING fnamele = folderPath;
  fnamele.append(_T("tesfile.le"));

  reflClass rclass = ReflectorTest();

  printline("Saving: ", << fnamele);

  {
    BinWritter wr(fnamele);
    wr.Write(static_cast<_ReflClassData &>(rclass));
  }

  TFileInfo fle(fnamele.c_str());
  TSTRING fnamebe = fle.GetPath() + fle.GetFileName() + _T(".be");

  printline("Saving: ", << fnamebe);

  {
    BinWritter wr(fnamebe);
    wr.SwapEndian(true); // continue write as big endian from now on
    wr.Write(static_cast<_ReflClassData &>(rclass));
  }

  reflClass rclassle;

  printline("Loading: ", << fnamele);

  {
    BinReader wr(fnamele);
    wr.Read(static_cast<_ReflClassData &>(rclassle));
  }

  reflClass rclassbe;

  printline("Loading: ", << fnamebe);

  {
    BinReader wr(fnamebe);
    wr.SwapEndian(true); // continue loading as big endian from now on
    wr.Read(static_cast<_ReflClassData &>(rclassbe));
  }
}

/************************************************************************/
/*********************** allocator_hybrid SAMPLE ************************/
/************************************************************************/
void HybridVector() {
  char *externalBuffer = static_cast<char *>(malloc(125));
  memset(externalBuffer, 89, 125);

  std::vector<char, std::allocator_hybrid<char>> vectr;
  vectr.resize(85);

  // assigning externalBuffer as internal buffer for vector
  // equivalent to: vectr = std::vector<char,
  // std::allocator_hybrid<char>>(externalBuffer, externalBuffer + 125,
  // std::allocator_hybrid<char>(externalBuffer));
  vectr = decltype(vectr)(externalBuffer, externalBuffer + 125,
                          decltype(vectr)::allocator_type(externalBuffer));

  vectr[12] = 12;

  decltype(vectr)::allocator_type::DisposeStorage(vectr);

  // doing normal vector stuff afterwards
  vectr.push_back(75);

  free(externalBuffer);
}

/************************************************************************/
/*********************** DISABLER SAMPLE ********************************/
/************************************************************************/
struct class01 {
  const int iType = 12;
};

struct class02 {
  void noType();
};

struct ImasterClass {
  virtual int Func01() = 0;
  virtual ~ImasterClass() {}
};

template <class _parent> struct masterClass_t : _parent {
  ADD_DISABLERS(_parent, noType);

  enabledFunction(noType, int) Func01() { return this->iType; }

  disabledFunction(noType, int) Func01() { return -1; }
};

template <class _Ty> struct masterClass : ImasterClass {
  typedef masterClass_t<_Ty> typuh;
  typuh Data;

  masterClass() {}

  int Func01() { return Data.Func01(); }
};

void DisablerTest() {
  std::unique_ptr<ImasterClass> cls01(new masterClass<class01>);
  std::unique_ptr<ImasterClass> cls02(new masterClass<class02>);

  int cls1out = cls01->Func01(); // = 12;
  int cls2out = cls02->Func01(); // = -1;
}

/************************************************************************/
/******** BINWRITTER, BINREADER, XORENCODER, MASTERPRINTER SAMPLE *******/
/************************************************************************/
void EncryptorTest(const TCHAR *folderPath) {
  TSTRING fnameenc = folderPath;
  fnameenc.append(_T("tesfile.enc"));

  reflClass rclass = ReflectorTest();

  const char *key = "Sample Key";

  printline("Saving: ", << fnameenc);

  {
    BinWritter wr(fnameenc);
    wr.Encryptor<XOREncoder>();
    wr.Encryptor()->SetKey(key, sizeof(key) - 1);
    wr.Write(static_cast<_ReflClassData &>(rclass));
  }

  reflClass rclassenc;

  printline("Loading: ", << fnameenc);

  {
    BinReader wr(fnameenc);
    wr.Encryptor<XOREncoder>();
    wr.Encryptor()->SetKey(key, sizeof(key) - 1);
    wr.Read(static_cast<_ReflClassData &>(rclassenc));
  }
}

/************************************************************************/
/*********************** ESSTRING, MASTERPRINTER SAMPLE *****************/
/************************************************************************/
void ESstringTest() {
  esString wide = L"Samle Text utf16";
  esString normal = "Sample Text default";

  printf(static_cast<std::string>(wide).c_str(), NULL);
  printf("\n");
  printer << wide >> 1;

  printf(static_cast<std::string>(normal).c_str(), NULL);
  printf("\n");
  printer << normal >> 1;
}

/************************************************************************/
/*********************** MASTERPRINTER THREAD SAMPLE ********************/
/************************************************************************/

void ThreadFunc() { printline("Hello, thread.") }

void MasterprinterTest() {
  const int nthreads = std::thread::hardware_concurrency();
  std::vector<std::thread> threadedfuncs(nthreads);
  printer.PrintThreadID(true);

  for (int t = 0; t < nthreads; t++)
    threadedfuncs[t] = std::thread(ThreadFunc);

  for (int t = 0; t < nthreads; t++)
    threadedfuncs[t].join();

  printer.PrintThreadID(false);
}

/************************************************************************/
/*********************** MAIN *******************************************/
/************************************************************************/
int main(const int argc, const TCHAR **argv) {
#ifndef UNICODE
  printer.AddPrinterFunction(reinterpret_cast<void *>(
      printf)); // adding console print function for masterprinter service
#else
  printer.AddPrinterFunction(reinterpret_cast<void *>(wprintf));
#endif
  printline("Compiler info:\n\tLittle Endian: ",
            << ES_LITTLE_ENDIAN << "\n\tX64: " << ES_X64
            << "\n\tClass padding optimalization: " << ES_REUSE_PADDING);

  printerror("I am error and I'm red.");
  printwarning("I am warning and I'm yellow.")

  DisablerTest();
  HybridVector();
  ReflectorTest();

  TSTRING folderpath = TFileInfo(argv[0]).GetPath();

  FileIO(folderpath.c_str());
  EncryptorTest(folderpath.c_str());

  ESstringTest();

  MasterprinterTest();

  DirectoryScanner scan;
  scan.Scan(folderpath);

  printline("Printing build directory.");

  for (auto &s : scan) {
    printer << s >> 1;
  }


  Vector4A16 tvec(argc + 1, argc + 2, argc + 3, argc + 4);
  tvec += 10; // 11, 12, 13, 14
  tvec /= 2; // 5.5, 6, 6.5, 7
  tvec -= 1; // 4.5, 5, 5.5, 6 
  tvec *= 1.5; // 6.75, 7.5, 8.25, 9
  Vector4A16 tvec2 = -tvec;



  float vLen = tvec.Length();
  float vLen2 = Vector4(6.75, 7.5, 8.25, 9).Length();

  float vDot = tvec.Dot(tvec);
  float vDot2 = Vector4(6.75, 7.5, 8.25, 9).Dot({6.75, 7.5, 8.25, 9});

  Vector4A16 tvec3(0.1, 0.2, 0.3, 0.4);
  bool res = tvec3 != tvec3 + FLT_EPSILON + (FLT_EPSILON / 2);

  bool sym = tvec.IsSymetrical();

  return reinterpret_cast<int&> (tvec.X);
}