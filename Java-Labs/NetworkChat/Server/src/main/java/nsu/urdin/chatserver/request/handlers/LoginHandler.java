package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.ErrorResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.SuccessResponse;
import nsu.urdin.chatprotocol.dto.event.LoginEvent;
import nsu.urdin.chatprotocol.dto.request.LoginRequest;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatserver.ChatServer;
import nsu.urdin.chatserver.ConnectionSession;

@Slf4j
public class LoginHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session) {
        if (requestDto instanceof LoginRequest loginRequest) {
            if (session.isLoggedIn()) {
                return new ErrorResponse("You are already logged in");
            }

            //TODO: check credentials in database, retrieve user from db

            User user = new User(loginRequest.getName());

            session.login(user);
            ChatServer.getInstance().broadcastEvent(new LoginEvent(user), session, true);
            return new SuccessResponse();
        }
        return null;
    }
}
