package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.dto.request.ChatHistoryRequest;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.response.ChatHistorySuccessResponse;
import nsu.urdin.chatprotocol.dto.response.ErrorResponse;
import nsu.urdin.chatprotocol.dto.response.NotAuthenticatedResponse;
import nsu.urdin.chatprotocol.dto.response.ResponseBase;
import nsu.urdin.chatserver.connection.ConnectionSession;
import nsu.urdin.chatserver.database.Database;
import nsu.urdin.chatserver.exception.DatabaseException;

public class GetChatHistoryHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session, Database db) {
        if (requestDto instanceof ChatHistoryRequest chatHistoryRequest) {
            if (!session.isLoggedIn()) {
                return new NotAuthenticatedResponse();
            }

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
