package nsu.urdin.chatprotocol.dto.request;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;
import nsu.urdin.chatprotocol.dto.DtoBase;

@Getter
public class RequestBase extends DtoBase {
    private final String commandName;

    @JsonCreator
    public RequestBase(@JsonProperty("commandName") String commandName) {
        this.commandName = commandName;
    }
}