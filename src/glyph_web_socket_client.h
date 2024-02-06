#ifndef GLYPHWEBSOCKETCLIENT_H
#define GLYPHWEBSOCKETCLIENT_H

#include <QWebSocket>
#include <QTimer>
#include <QElapsedTimer>

#include "composition_manager.h"

// Logging
#include <QLoggingCategory>
Q_DECLARE_LOGGING_CATEGORY(glyphWebSocketClient)
Q_DECLARE_LOGGING_CATEGORY(glyphWebSocketClientVerbose)

class GlyphWebSocketClient : public QWebSocket
{
    Q_OBJECT
public:
    explicit GlyphWebSocketClient(QObject *parent = nullptr);
    ~GlyphWebSocketClient();

    inline CompositionManager::PhoneModel getCurrentlyConnectedModel() const { return this->currentlyConnectedModel; }

private:
    CompositionManager::PhoneModel currentlyConnectedModel = CompositionManager::PhoneModel::None;

    /**
     * @brief Holds after how many seconds a heartbeat should be sent.
     */
    static constexpr qint64 CONNECTION_LOST_HEARTBEAT_MS = 2000;
    /**
     * @brief Holds after how many seconds the connection is considered lost after no pong reply.
     */
    static constexpr qint64 CONNECTION_LOST_TIMEOUT_MS = CONNECTION_LOST_HEARTBEAT_MS * 1.5;

    /**
     * @brief Will trigger the heartbeat by firstly closing the socket if a pong has not been sent recently enough and secondly sending a ping to the server.
     */
    QTimer *connectionLostHeartbeatTimer;
    /**
     * @brief Will count the time elapsed since the ping was sent and the pong was received.
     */
    QElapsedTimer *connectionLostTimeoutTimer;
};

#endif // GLYPHWEBSOCKETCLIENT_H
