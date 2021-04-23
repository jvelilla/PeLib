#ifndef DotNetPELib_METHODSIGNATURE
#define DotNetPELib_METHODSIGNATURE

#include <PeLib/Resource.h>
#include <deque>

namespace DotNetPELib
{
    class Type;
    class DataContainer;
    class Param;
    class PELib;
    class AssemblyDef;
    class PEReader;
    class Method;

    ///** the signature for a method, has a return type and a list of params.
    // params can be either named or unnamed
    // if the signature is not managed it is PINVOKE
    // There are two types of vararg protocols supported.
    // When performing a PINVOKE, the native CIL vararg mechanism is used
    // because that is how things are marshalled.   But if varars are used
    // in the arguments to a managed function, the argument list will end with
    // an argument which is an szarray of objects.  It will be tagged appropriately
    // so that other .net assemblies e.g. C# programs know how to use it as a param
    // list, including the ability to specify an arbitrary number of params.
    // When these are passed about in a program you generate you may need to box
    // simple values to fit them in the array...
    class MethodSignature : public DestructorBase
    {
    public:
        enum { Vararg = 1, Managed = 2, InstanceFlag = 4 };
        MethodSignature(const std::string& Name, int Flags, DataContainer *Container) : container_(Container), name_(Name), flags_(Flags), returnType_(nullptr), ref_(false),
                peIndex_(0), peIndexCallSite_(0), peIndexType_(0), methodParent_(nullptr), arrayObject_(nullptr), external_(false), definitions_(0), genericParent_(nullptr), genericParamCount_(0)
        {
        }
        MethodSignature(const MethodSignature&) = default;
        MethodSignature& operator=(const MethodSignature&) = default;

        ///** Get return type
        Type *ReturnType() const { return returnType_; }
        ///** Set return type
        void ReturnType(Type *type)
        {
            returnType_ = type;
        }
        //* * Add a parameter.  They need to be added in order.
        void AddParam(Param *param);
        ///** Add a vararg parameter.   These are NATIVE vararg parameters not
        // C# ones.   They are only added to signatures at a call site...
        void AddVarargParam(Param *param);
        ///** This is the parent declaration for a call site signature with vararg
        // params (the methoddef version of the signature)
        void SignatureParent(MethodSignature *parent) { methodParent_ = parent; }
        ///** return the parent declaration for a call site signature with vararg
        // params (the methoddef version of the signature)
        MethodSignature *SignatureParent() { return methodParent_; }
        ///** Set the data container
        void SetContainer(DataContainer *container) { container_ = container; }
        ///** Get the data container
        DataContainer *GetContainer() const { return container_; }
        ///** Get name
        const std::string &Name() const { return name_; }
        ///** Set name
        void SetName(const std::string& Name) { name_ = Name; }
        ///** Set Array object
        void ArrayObject(Type *tp) { arrayObject_ = tp; }
        // iterate through parameters
        typedef std::list<Param *>::iterator iterator;
        iterator begin() { return params.begin(); }
        iterator end() { return params.end(); }

        void GenericParent(MethodSignature* sig) { genericParent_ = sig; }
        MethodSignature* GenericParent() const { return genericParent_; }
        ///** return the list of generics
        std::deque<Type*>& Generic() { return generic_; }
        const std::deque<Type*>& Generic() const { return generic_; }

        // iterate through vararg parameters
        typedef std::list<Param *>::iterator viterator;
        iterator vbegin() { return varargParams_.begin(); }
        iterator vend() { return varargParams_.end(); }
        ///** make it an instance member
        void Instance(bool instance) {
            if (instance)
            {
                flags_ |= InstanceFlag;
            }
            else
                flags_ &= ~InstanceFlag;
        }
        bool Instance() const { return !!(flags_ & InstanceFlag); }

        // make it virtual
        ///** make it a vararg signature
        void SetVarargFlag() { flags_ |= Vararg; }

        ///** return qualifiers
        int Flags() const { return flags_; }
        ///** return parameter count
        size_t ParamCount() const { return params.size(); }
        ///** return vararg parameter count
        size_t VarargParamCount() const { return varargParams_.size(); }
        ///** not locally defined
        bool External() const { return external_; }
        ///** not locally defined
        void External(bool external) { external_ = external; }
        ///** increment definition count
        void Definition() { definitions_++; }
        ///** get definition count
        size_t Definitions() const { return definitions_/2; }
        size_t GenericParamCount() const { return genericParamCount_; }
        void GenericParamCount(int count) { genericParamCount_ = count; }
        bool MatchesType(Type *tpa, Type *tpp);
        bool Matches(std::vector<Type *> args);
        // various indexes into metadata tables
        size_t PEIndex() const { return peIndex_; }
        void PEIndex(size_t val) { peIndex_ = val;  }
        size_t PEIndexCallSite() const { return peIndexCallSite_; }
        size_t PEIndexType() const { return peIndexType_; }

        // Load a signature
        void Load(PELib &lib, AssemblyDef &assembly, PEReader &reader, int start, int end);
        // internal functions
        void Ref(bool Ref) { ref_ = Ref; }
        bool IsRef() const { return ref_; }
        void ILSignatureDump(PELib &peLib);
        virtual bool ILSrcDump(PELib &, bool names, bool asType, bool PInvoke) const;
        virtual bool PEDump(PELib &, bool asType);
        virtual void ObjOut(PELib &, int pass) const;
        static MethodSignature *ObjIn(PELib &, Method **found, bool definition = true);
        std::string AdornGenerics(PELib& peLib, bool names = false) const;

    protected:
        MethodSignature *methodParent_;
        DataContainer *container_;
        Type *returnType_;
        Type *arrayObject_;
        std::string name_, display_name;
        int flags_;
        std::list<Param *> params, varargParams_;
        bool ref_;
        size_t peIndex_, peIndexCallSite_, peIndexType_;
        bool external_;
        size_t definitions_;
        std::deque<Type*> generic_;
        MethodSignature* genericParent_;
        int genericParamCount_;
    };
}

#endif // DotNetPELib_METHODSIGNATURE

