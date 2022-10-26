#include "../PublicApi.h"
#include <iomanip>
#include <QtCore/QCoreApplication>
#include <QtCore/QtDebug>
#include "../SimpleApi.h"
using namespace DotNetPELib;


void SplitPath(std::vector<std::string>& split, std::string path)
{
    std::string last;
    int n = path.find(":");
    if (n != std::string::npos && n < path.size() - 2 && path[n + 1] == ':')
    {
        last = path.substr(n + 2);
        path = path.substr(0, n);
    }
    n = path.find(".");
    while (n != std::string::npos)
    {
        split.push_back(path.substr(0, n));
        if (path.size() > n + 1)
            path = path.substr(n + 1);
        else
            path = "";
        n = path.find(".");
    }
    if (path.size())
    {
        split.push_back(path);
    }
    if (last.size())
    {
        split.push_back(last);
    }
    if (split.size() > 2)
    {
        if (split[split.size() - 1] == "ctor" || split[split.size() - 1] == "cctor")
            if (split[split.size() - 2] == "")
            {
                split[split.size() - 2] = "." + split[split.size() - 1];
                split.resize(split.size() - 1);
            }
    }
}


std::string EscapedString(std::string stringValue_) 
{
    bool doit = false;
    for (unsigned i : stringValue_)
        if (i < 32 || i > 126 || i == '\\' || i == '"')
        {
            doit = true;
            break;
        }
    if (doit)
    {
        std::string retVal;
        for (unsigned i : stringValue_)
        {
            i &= 0xff;
            if (i < 32)
            {
                switch (i)
                {

                case '\a':
                    i = 'a';
                    break;
                case '\b':
                    i = 'b';
                    break;
                case '\f':
                    i = 'f';
                    break;
                case '\n':
                    i = 'n';
                    break;
                case '\r':
                    i = 'r';
                    break;
                case '\v':
                    i = 'v';
                    break;
                case '\t':
                    i = 't';
                    break;
                default:
                    break;
                }
                if (i < 32)
                {

                    retVal += "\\0";
                    retVal += ((i / 8) + '0');
                    retVal += ((i & 7) + '0');
                }
                else
                {
                    retVal += "\\";
                    retVal += (char)i;
                }
            }
            else if (i == '"' || i == '\\')
            {
                retVal += "\\";
                retVal += (char)i;
            }
            else if (i > 126)
            {
                retVal += "\\";
                retVal += ((i / 64) + '0');
                retVal += (((i / 8) & 7) + '0');
                retVal += ((i & 7) + '0');
            }
            else
                retVal += i;
        }
        return retVal;
    }
    return stringValue_;
}


void test2()
{
    SimpleApi api;

    try
    {
        api.beginModule("HelloWorld");
        api.addModuleReference("mscorlib");
        api.mscorlib();
        api.beginMethod("Main", true, SimpleApi::Primary);
        api.LDSTR("Hello World!");
        api.CALL("System.Console.WriteLine(string)");
        api.RET();
        api.endMethod();
        api.writeByteCode("test2.exe");
        api.writeAssembler("test2.il");
        api.endModule();
    }
    catch (const std::runtime_error& e)
    {
        qCritical() << e.what();
    }
}

void test3()
{
    SimpleApi api;

    try
    {
        api.beginModule("Test3");
        api.beginNamespace("Gugus");
        api.addEnum("Color", true, SimpleApi::EnumItems() << SimpleApi::EnumItem("red", 10) <<
            SimpleApi::EnumItem("blue", 20) << SimpleApi::EnumItem("green", 30), true);
        api.addModuleReference("mscorlib");
        api.beginMethod("Main", true, SimpleApi::Primary);
        //api.LDC(12345);
        api.LDSFLD("Gugus.Color.green");
        api.CALL("System.Console.WriteLine(int32)");
        api.RET();
        api.endMethod();
        api.endNamespace();
        api.writeByteCode("test3.exe");
        api.writeAssembler("test3.il");
        api.endModule();
    }
    catch (const std::runtime_error& e)
    {
        qCritical() << e.what();
    }
}

void test4()
{
    SimpleApi api;

    try
    {
        api.beginModule("Test4");
        api.addModuleReference("mscorlib");

        api.addField("myString", "string", false, true);

        api.beginMethod(".cctor", false, SimpleApi::Static);
        api.LDSTR("Hello from .cctor!");
        api.STSFLD("myString");
        api.RET();
        api.endMethod();

        api.beginMethod("Test", false, SimpleApi::Static);
        api.addArgument("string&");
        api.LDARG(0);
        api.LDSTR("Hello from Test1!");
        api.STIND(SimpleApi::Ref);
        api.RET();
        api.endMethod();

        api.beginMethod("Main", true, SimpleApi::Primary);
        api.LDSFLD("myString");
        api.CALL("System.Console.WriteLine(string)");
        api.LDSFLDA("myString");
        api.CALL("Test");
        api.LDSFLD("myString");
        api.CALL("System.Console.WriteLine(string)");
        api.RET();
        api.endMethod();

        //api.writeByteCode("test4.exe");
        api.writeAssembler("test4.il");
        api.endModule();
    }
    catch (const std::runtime_error& e)
    {
        qCritical() << e.what();
    }
}

void test5()
{
    SimpleApi api;

    try
    {
        api.beginModule("Test5");
        api.addModuleReference("mscorlib");

        api.beginClass("Outer", true);

        api.beginClass("Inner", true);

        api.beginMethod("DoIt", true, SimpleApi::Static);
        api.addArgument("string");
        api.LDARG(0);
        api.CALL("System.Console.WriteLine(string)");
        api.RET();
        api.endMethod();

        api.endClass();

        api.endClass();

        api.beginMethod("Main", true, SimpleApi::Primary);
        api.LDSTR("This is a string");
        api.CALL("Outer/Inner::DoIt");
        api.RET();
        api.endMethod();

        api.writeByteCode("test5.exe");
        api.writeAssembler("test5.il");
        api.endModule();
    }
    catch (const std::runtime_error& e)
    {
        qCritical() << e.what();
    }
}

void test6()
{
    SimpleApi api;

    try
    {
        api.beginModule("Test6");
        api.addModuleReference("mscorlib");

        api.addField("array1", "int8[]", true, true);

        api.beginStruct("MyStruct");
        api.addField("alpha", "string");
        api.addField("beta", "int32");
        api.addField("gamma", "uint16[]");
        api.endStruct();

        api.addField("array2", "MyStruct[]", false, true);

        api.beginMethod("Main", true, SimpleApi::Primary);
        api.addLocal("MyStruct");

        api.LDC(10);
        api.NEWARR("int8[]");
        api.STSFLD("array1");
        api.LDC(3);
        api.NEWARR("MyStruct[]");
        api.STSFLD("array2");

        api.addField("array3", "int16[,]", false, false);
        api.LDTOKEN("System.Int16");
        api.CALL("System.Type::GetTypeFromHandle(System.RuntimeTypeHandle)");
        api.LDC(3);
        api.LDC(4);
        api.CALL("System.Array::CreateInstance(System.Type, int32, int32)");
        api.STSFLD("array3");

        api.LDSFLD("array3");
        api.LDC(123);
        api.BOX("System.Int16");
        api.LDC(1);
        api.LDC(2);
        api.CALLVIRT("System.Array::SetValue(object, int32, int32)");

        api.LDSFLD("array3");
        api.LDC(1);
        api.LDC(2);
        api.CALLVIRT("System.Array::GetValue(int32,int32)");
        api.UNBOX("System.Int16");
        api.CALL("System.Console.WriteLine(int32)");

        api.RET();
        api.endMethod();

        api.writeByteCode("test6.exe");
        api.writeAssembler("test6.il");
        api.endModule();
    }
    catch (const std::runtime_error& e)
    {
        qCritical() << e.what();
    }
}


int main()
{
    //test2();

    //Byte buf[8];

    //double floatValue_ = 3.5 ;
    //*(float*)buf = floatValue_;


    //std::string stretVal;
    //int i = 7;
    ////stretVal += "\\0";
    //std::cout << 10 / 8 << std::endl;
    //stretVal += ((i / 8) + '0');
    //stretVal += ((i & 7) + '0');
    //stretVal += "\\0";
    //std::cout << stretVal;

    //std::cout << EscapedString("Do you wish to send anyway?") << std::endl;
    //std::cout << EscapedString("In the last decade, the German word \"über\" has come to be used frequently in colloquial English.");

    //int offset = 0;
    //int foo[5] = { 16, 2, 77, 40, 12071 };
    //std::cout << foo[offset++];
    //Instruction* old = nullptr;
    //if (!old) 
    //{
    //    std::cout << "true";
    //}
    //std::vector<std::string> split;
    //SplitPath(split, "System::Console.WriteLine");
    //char cbuf[256];

    //std::string Name = cbuf;
    PELib peFile("test1");
    

    AssemblyDef* assembly = peFile.WorkingAssembly();
    MethodSignature* sigMain = new MethodSignature("$Main", MethodSignature::Managed, assembly);
    sigMain->ReturnType(new Type(Type::Void));
    Method* methMain = new Method(sigMain, Qualifiers::Private |
        Qualifiers::Static |
        Qualifiers::HideBySig |
        Qualifiers::CIL |
        Qualifiers::Managed, true);
    assembly->Add(methMain);


    AssemblyDef* mscorlib = peFile.MSCorLibAssembly();

    Resource* r;
    Class* console;
#if 1
    Namespace* system;
    peFile.Find("System", &r);
    system = (Namespace*)r;
    if (peFile.Find("Console", &r) != PELib::s_class)
    {
        console = new Class("Console", Qualifiers::Public, -1, -1);
        system->Add(console);
    }
    else
        console = (Class*)r;
#else
    console = new Class("System.Console", Qualifiers::Public, -1, -1);
    mscorlib->Add(console);
#endif

    MethodSignature* sigWriteLine = new MethodSignature("WriteLine", MethodSignature::Managed, console);
    sigWriteLine->ReturnType(new Type(Type::Void));
    sigWriteLine->AddParam(new Param("", new Type(Type::string)));


    methMain->AddInstruction(
        new Instruction(Instruction::i_ldstr, new Operand("Hi there!", true)));
    methMain->AddInstruction(
        new Instruction(Instruction::i_call, new Operand(new MethodName(sigWriteLine))));
    methMain->AddInstruction(
        new Instruction(Instruction::i_ret));

    try
    {
        methMain->Optimize();
    }
    catch (PELibError exc)
    {
        std::cout << "Optimizer error: " << exc.what();
    }

    peFile.DumpOutputFile("HiThere.il", PELib::ilasm, false);
    peFile.DumpOutputFile("HiThere.exe", PELib::peexe, false);


}


