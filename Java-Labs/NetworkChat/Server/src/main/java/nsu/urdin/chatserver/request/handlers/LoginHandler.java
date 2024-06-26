package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.event.LoginEvent;
import nsu.urdin.chatprotocol.dto.request.LoginRequest;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.response.ErrorResponse;
import nsu.urdin.chatprotocol.dto.response.ResponseBase;
import nsu.urdin.chatprotocol.dto.response.SuccessResponse;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatserver.ChatServer;
import nsu.urdin.chatserver.connection.ConnectionSession;
import nsu.urdin.chatserver.database.Database;
import nsu.urdin.chatserver.exception.DatabaseException;

import java.util.Optional;

@Slf4j
public class LoginHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session, Database db) {
        if (requestDto instanceof LoginRequest loginRequest) {
            if (session.isLoggedIn()) {
                return new ErrorResponse("You are already logged in");
            }

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
