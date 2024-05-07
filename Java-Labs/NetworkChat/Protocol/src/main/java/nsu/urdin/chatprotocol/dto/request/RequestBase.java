package nsu.urdin.chatprotocol.dto.request;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.io.Serializable;

@Getter
@AllArgsConstructor
public class RequestBase implements Serializable {
    private final String commandName;
}