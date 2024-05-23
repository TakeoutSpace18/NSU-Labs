package nsu.urdin.chatserver.request.handlers;

import nsu.urdin.chatprotocol.dto.ErrorResponse;
import nsu.urdin.chatprotocol.dto.ResponseBase;
import nsu.urdin.chatprotocol.dto.SuccessResponse;
import nsu.urdin.chatprotocol.dto.request.RegisterRequest;
import nsu.urdin.chatprotocol.dto.request.RequestBase;
import nsu.urdin.chatprotocol.entity.User;
import nsu.urdin.chatprotocol.exception.ValidationException;
import nsu.urdin.chatserver.ConnectionSession;
import nsu.urdin.chatserver.database.Database;
import nsu.urdin.chatserver.exception.DatabaseException;

public class RegisterHandler extends RequestHandler {

    @Override
    public ResponseBase handle(RequestBase requestDto, ConnectionSession session, Database db) {
        if (requestDto instanceof RegisterRequest registerRequest) {
            try {
                if (db.findUser(registerRequest.getName()).isPresent()) {
                    return new ErrorResponse("Username already exists");
                }

                validatePassword(registerRequest.getPassword());

                User user = new User(registerRequest.getName());
                db.addUser(user, registerRequest.getPassword());
            } catch (DatabaseException | ValidationException e) {
                return new ErrorResponse(e.getMessage());
            }

            return new SuccessResponse();
        }
        return null;
    }

    private void validatePassword(String password) throws ValidationException {
        if (password.isEmpty()) {
            throw new ValidationException("Password should not be empty");
        }
    }
}
