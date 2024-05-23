package nsu.urdin.chatprotocol.dto.response;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;
import nsu.urdin.chatprotocol.dto.DtoBase;

@Getter
public class ResponseBase extends DtoBase {
    private final String status;

    @JsonCreator
    public ResponseBase(@JsonProperty("status") String status) {
        this.status = status;
    }
}
