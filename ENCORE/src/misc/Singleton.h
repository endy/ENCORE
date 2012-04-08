//////////////////////////////////////////////////////////////////////////
// FRAMEWORK
// 
// Framework description and license goes here.
// 
// Developed By
//      Brandon Light:   brandon.light@gmail.com
//////////////////////////////////////////////////////////////////////////

template <class T> class Singleton
{  
protected:
    Singleton(){}

public:
    virtual ~Singleton(){ Release(); }

    static void Create();
    static void Release();

    static T& Get(){ return *m_Instance; }  

private:
    static T* m_Instance;

};

template <class T> T *Singleton<T>::m_Instance = NULL;


// creates an instance of T
template <class T> void Singleton<T>::Create()
{
    Release();
    m_Instance = new T();
}

template <class T> void Singleton<T>::Release()
{
    if(m_Instance)
    {
        delete m_Instance;
        m_Instance = NULL;
    }
}

