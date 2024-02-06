#include "glyph_web_socket_client.h"

// Logging
Q_LOGGING_CATEGORY(glyphWebSocketClient, "GlyphWebSocketClient")
Q_LOGGING_CATEGORY(glyphWebSocketClientVerbose, "GlyphWebSocketClient.Verbose")

GlyphWebSocketClient::GlyphWebSocketClient(QObject *parent)
    : QWebSocket{QString(), QWebSocketProtocol::Version::VersionLatest, parent}
{
    // Configure connection lost detection
    this->connectionLostTimeoutTimer = new QElapsedTimer();

    this->connectionLostHeartbeatTimer = new QTimer(this);
    this->connectionLostHeartbeatTimer->setTimerType(Qt::TimerType::CoarseTimer);
    connect(this->connectionLostHeartbeatTimer, &QTimer::timeout, this, [=](){
        // Check if the web socket is valid
        if (!this->isValid())
            return;

        // Check the timeout
        qCInfo(glyphWebSocketClientVerbose) << this->connectionLostTimeoutTimer->elapsed() << ">" << CONNECTION_LOST_TIMEOUT_MS;
        if (this->connectionLostTimeoutTimer->elapsed() > CONNECTION_LOST_TIMEOUT_MS)
        {
            qCWarning(glyphWebSocketClient) << "Hearbeat timeout, lost connection";
            this->close(QWebSocketProtocol::CloseCode::CloseCodeAbnormalDisconnection, "Lost connection timeout");
            return;
        }

        // Send a ping if no timeout occured
        this->ping();
    });

    // Make connections
    connect(this, &QWebSocket::connected, this, [=](){
        qCInfo(glyphWebSocketClient) << "Device connected, starting heartbeat";
        // Reset and start the heartbeat timer
        this->connectionLostHeartbeatTimer->start(CONNECTION_LOST_HEARTBEAT_MS);
        this->connectionLostTimeoutTimer->start();
    });
    connect(this, &QWebSocket::disconnected, this, [=](){
        qCInfo(glyphWebSocketClient) << "Device disconnected, stoping heartbeat";
        // Stop the heartbeat timer
        this->connectionLostHeartbeatTimer->stop();
        // Reset the currentlyConnectedModel
        this->currentlyConnectedModel = CompositionManager::PhoneModel::None;
    });
    connect(this, &QWebSocket::pong, this, [=](quint64 elapsedTime, const QByteArray &payload){
        qCInfo(glyphWebSocketClientVerbose) << "Received pong -" << elapsedTime;
        // Just reset the timeout timer. In normal operation this should not trigger the timeout.
        this->connectionLostTimeoutTimer->start();
    });
    connect(this, &QWebSocket::textMessageReceived, this, [=](const QString& message){
        // Parse the sent phone model
        if (!message.startsWith("Model="))
            return;

        QString modelName = message.last(message.length() - QString("Model=").length());
        qCInfo(glyphWebSocketClient) << "Device sent model" << modelName;
        if (modelName == CompositionManager::getPhoneModelString(CompositionManager::PhoneModel::Phone1))
        {
            this->currentlyConnectedModel = CompositionManager::PhoneModel::Phone1;
            return;
        }
        if (modelName == CompositionManager::getPhoneModelString(CompositionManager::PhoneModel::Phone2))
        {
            this->currentlyConnectedModel = CompositionManager::PhoneModel::Phone2;
            return;
        }

        // Invalid phone model
        qCWarning(glyphWebSocketClient) << "Invalid model!!";
        this->currentlyConnectedModel = CompositionManager::PhoneModel::None;
    });
}

GlyphWebSocketClient::~GlyphWebSocketClient()
{
    delete this->connectionLostTimeoutTimer;
}
