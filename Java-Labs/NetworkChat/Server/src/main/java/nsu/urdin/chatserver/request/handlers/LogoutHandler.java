package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.dto.request.LogoutRequest;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.response.NotAuthenticatedResponse;
import nsu.urdin.chatprotocol.dto.response.ResponseBase;
import nsu.urdin.chatprotocol.dto.response.SuccessResponse;
import nsu.urdin.chatserver.ChatServer;
import nsu.urdin.chatserver.connection.ConnectionSession;
import nsu.urdin.chatserver.database.Database;

public class LogoutHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session, Database db) {
        if (requestDto instanceof LogoutRequest logoutRequest) {
            if (!session.isLoggedIn()) {
                return new NotAuthenticatedResponse();
            }

            ChatServer.getInstance().closeSession(session);

            return new SuccessResponse();
        }
        return null;
    }
}
