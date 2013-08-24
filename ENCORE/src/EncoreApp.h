///////////////////////////////////////////////////////////////////////////////////////////////////
///
///     ENCORE Rendering App
///
///     Copyright 2013, Brandon Light
///     All rights reserved.
///
///////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _ENCOREAPP_H_
#define _ENCOREAPP_H_

#include "IvyApp.h"


class EncoreApp : public Ivy::IvyApp
{
public:
    static bool Configure(unsigned int argc, const char** argv);

    static EncoreApp* Create();

    virtual void Destroy();

    virtual void Run();

protected:
    EncoreApp();
    virtual ~EncoreApp();

    virtual bool Init();

    virtual void ReceiveEvent(const Event* pEvent);

    void ProcessKeyUpdates();
    void ProcessMouseUpdates();

private:
    EncoreApp(const EncoreApp& copy); // disallow copy constructor

    BOOL m_useEgl;
};

#endif // _ENCOREAPP_H_
