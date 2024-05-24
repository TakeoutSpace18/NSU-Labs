package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.response.ResponseBase;
import nsu.urdin.chatserver.connection.ConnectionSession;
import nsu.urdin.chatserver.database.Database;

@Slf4j
public abstract class RequestHandler {
    public abstract ResponseBase handle(RequestBase requestDto, ConnectionSession session, Database db);
}
