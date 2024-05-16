package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.SuccessResponse;
import nsu.urdin.chatprotocol.dto.request.RegisterRequest;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatserver.ConnectionSession;

public class RegisterHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session) {
        if (requestDto instanceof RegisterRequest registerRequest) {
            // TODO: add to database

            return new SuccessResponse();
        }
        return null;
    }
}
