package nsu.urdin.chatclient;

import nsu.urdin.chatprotocol.entity.Message;
import nsu.urdin.chatprotocol.entity.User;

public interface ChatEventListener {
    void onNewMessage(Message message);
    void onUserLogin(User user);
    void onUserLogout(User user);
}
