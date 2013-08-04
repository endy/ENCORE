

#include "IvyApp.h"


class EncoreApp : public Ivy::IvyApp
{
public:
    static EncoreApp* Create();

    virtual void Destroy();

    virtual void Run();

protected:
    EncoreApp();
    virtual ~EncoreApp();

    virtual bool Init();

    virtual void ReceiveEvent(const Event* pEvent);

private:
    EncoreApp(const EncoreApp& copy); // disallow copy constructor

    BOOL m_useEgl;
};
