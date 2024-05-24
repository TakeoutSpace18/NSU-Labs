package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.request.UsersListRequest;
import nsu.urdin.chatprotocol.dto.response.NotAuthenticatedResponse;
import nsu.urdin.chatprotocol.dto.response.ResponseBase;
import nsu.urdin.chatprotocol.dto.response.UsersListSuccessResponse;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatserver.ChatServer;
import nsu.urdin.chatserver.connection.ConnectionSession;
import nsu.urdin.chatserver.database.Database;

import java.util.List;

@Slf4j
public class GetUsersListHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session, Database db) {
        if (requestDto instanceof UsersListRequest usersListRequest) {
            if (!session.isLoggedIn()) {
                return new NotAuthenticatedResponse();
            }

            List<User> usersList = ChatServer.getInstance().getOnlineUsersList();

            return UsersListSuccessResponse.builder()
                    .users(usersList)
                    .build();
        }
        return null;
    }
}
