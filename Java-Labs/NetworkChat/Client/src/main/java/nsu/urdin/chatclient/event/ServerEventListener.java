package nsu.urdin.chatclient.event;

import nsu.urdin.chatprotocol.dto.event.LoginEvent;
import nsu.urdin.chatprotocol.dto.event.LogoutEvent;
import nsu.urdin.chatprotocol.dto.event.MessageEvent;

public interface ServerEventListener {
    void onNewMessage(MessageEvent event);
    void onUserLogin(LoginEvent event);
    void onUserLogout(LogoutEvent event);
}
