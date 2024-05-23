package nsu.urdin.chatprotocol.entity;

import lombok.Getter;
import lombok.NonNull;
import nsu.urdin.chatprotocol.exception.ValidationException;

import java.io.Serializable;

@Getter
public class User implements Serializable {
    @NonNull
    private final String name;

    public User(@NonNull String name) throws ValidationException {
        validateName(name);
        this.name = name;
    }

    private void validateName(String name) throws ValidationException {
        if (name == null || name.isEmpty()) {
            throw new ValidationException("Username cannot be empty");
        }
    }
}
