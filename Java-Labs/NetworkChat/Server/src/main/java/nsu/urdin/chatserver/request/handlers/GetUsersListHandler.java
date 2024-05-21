package nsu.urdin.chatserver.request.handlers;

import lombok.extern.slf4j.Slf4j;
import nsu.urdin.chatprotocol.dto.NotAuthenticatedResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.dto.request.UsersListRequest;
import nsu.urdin.chatprotocol.dto.request.UsersListSuccessResponse;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatserver.ChatServer;
import nsu.urdin.chatserver.ConnectionSession;

import java.util.List;

@Slf4j
public class GetUsersListHandler extends RequestHandler {
    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session) {
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
