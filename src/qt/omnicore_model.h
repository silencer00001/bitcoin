#ifndef OMNICORE_QT_MODEL_H
#define OMNICORE_QT_MODEL_H

#include <QObject>

/** Model for Omni Core related extensions. */
class OmniModel : public QObject
{
    Q_OBJECT

public:
    explicit ClientModel(QObject* parent = 0);
    ~ClientModel();

private:
    /** Connect core signals to GUI client */
    void subscribeToCoreSignals();
    /** Disconnect core signals from GUI client */
    void unsubscribeFromCoreSignals();

signals:
    void refreshOmniState();
    void refreshOmniPending(bool pending);
    void refreshOmniWallet();

public slots:
    void updateOmniState();
    void updateOmniPending(bool pending);
    void updateOmniWallet();
};


#endif // OMNICORE_QT_MODEL_H
