#include "omnicore_model.h"

#include "ui_interface.h"

#include <QDebug>
#include <QObject>

OmniModel::ClientModel(QObject* parent) : QObject(parent)
{
    subscribeToCoreSignals();
}

OmniModel::~ClientModel()
{
    unsubscribeFromCoreSignals();
}

void OmniModel::updateOmniState()
{
    emit refreshOmniState();
}

void OmniModel::updateOmniPending(bool pending)
{
    emit refreshOmniPending(pending);
}

void OmniModel::updateOmniWallet()
{
    emit refreshOmniWallet();
}

/** Triggered by each block that contains Omni layer transactions. */
static void OmniStateChanged(OmniModel* model)
{
    QMetaObject::invokeMethod(model, "updateOmniState", Qt::QueuedConnection);
}

/** Triggered when Omni pending map adds/removes transactions. */
static void OmniPendingChanged(OmniModel* model, bool pending)
{
    QMetaObject::invokeMethod(model, "updateOmniPending", Qt::QueuedConnection, Q_ARG(bool, pending));
}

/** Triggered when wallet balances have changed. */
static void OmniWalletChanged(OmniModel* model)
{
    qDebug() << "OmniWalletChanged";
    QMetaObject::invokeMethod(model, "updateOmniWallet", Qt::QueuedConnection);
}

/** Connect signals to client. */
void OmniModel::subscribeToCoreSignals()
{
    uiInterface.OmniStateChanged.connect(boost::bind(OmniStateChanged, this));
    uiInterface.OmniPendingChanged.connect(boost::bind(OmniPendingChanged, this, _1));
    uiInterface.OmniWalletChanged.connect(boost::bind(OmniWalletChanged, this));
}

/** Disconnect signals from client. */
void OmniModel::unsubscribeFromCoreSignals()
{
    uiInterface.OmniStateChanged.disconnect(boost::bind(OmniStateChanged, this));
    uiInterface.OmniPendingChanged.disconnect(boost::bind(OmniPendingChanged, this, _1));
    uiInterface.OmniWalletChanged.disconnect(boost::bind(OmniWalletChanged, this));
}

