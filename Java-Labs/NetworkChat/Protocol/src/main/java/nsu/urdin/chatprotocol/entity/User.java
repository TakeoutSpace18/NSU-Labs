package nsu.urdin.chatprotocol.entity;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;
import lombok.NonNull;
import nsu.urdin.chatprotocol.exception.ValidationException;

import java.io.Serializable;

@Getter
public class User implements Serializable {
    @NonNull
    private final String name;

    @JsonCreator
    public User(@NonNull @JsonProperty("name") String name) throws ValidationException {
        validateName(name);
        this.name = name;
    }

    private void validateName(String name) throws ValidationException {
        if (name == null || name.isEmpty()) {
            throw new ValidationException("Username cannot be empty");
        }
    }
}
