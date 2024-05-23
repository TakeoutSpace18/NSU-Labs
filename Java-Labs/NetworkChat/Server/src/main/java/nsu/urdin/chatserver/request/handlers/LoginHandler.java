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
import nsu.urdin.chatserver.database.Database;
import nsu.urdin.chatserver.exception.DatabaseException;

import java.util.Optional;

@Slf4j
public class LoginHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session) {
        if (requestDto instanceof LoginRequest loginRequest) {
            if (session.isLoggedIn()) {
                return new ErrorResponse("You are already logged in");
            }

            Database db = ChatServer.getInstance().getDatabase();

            try {
                Optional<User> user = db.findUser(loginRequest.getName());
                if (user.isEmpty()) {
                    return new ErrorResponse("User not found");
                }

                if (!db.authenticateUser(loginRequest.getName(), loginRequest.getPassword())) {
                    return new ErrorResponse("Incorrect password");
                }

                session.login(user.get());
                ChatServer.getInstance().broadcastEvent(new LoginEvent(user.get()), session, true);
            } catch (DatabaseException e) {
                return new ErrorResponse(e.getMessage());
            }

            return new SuccessResponse();
        }
        return null;
    }
}
