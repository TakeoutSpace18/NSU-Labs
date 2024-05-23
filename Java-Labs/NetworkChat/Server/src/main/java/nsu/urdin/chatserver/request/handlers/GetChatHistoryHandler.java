package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.dto.ErrorResponse;
import nsu.urdin.chatprotocol.dto.NotAuthenticatedResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.ChatHistoryRequest;
import nsu.urdin.chatprotocol.dto.request.ChatHistorySuccessResponse;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatserver.ChatServer;
import nsu.urdin.chatserver.ConnectionSession;
import nsu.urdin.chatserver.database.Database;
import nsu.urdin.chatserver.exception.DatabaseException;

public class GetChatHistoryHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session) {
        if (requestDto instanceof ChatHistoryRequest chatHistoryRequest) {
            if (!session.isLoggedIn()) {
                return new NotAuthenticatedResponse();
            }

            Database db = ChatServer.getInstance().getDatabase();

            try {
                db.getMessages();
                return new ChatHistorySuccessResponse(db.getMessages());
            } catch (DatabaseException e) {
                return new ErrorResponse(e.getMessage());
            }
        }

        return null;
    }
}
