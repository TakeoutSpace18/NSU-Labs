package nsu.urdin.chatprotocol.dto.event;

import com.fasterxml.jackson.annotation.JsonCreator;
import com.fasterxml.jackson.annotation.JsonProperty;
import lombok.Getter;
import nsu.urdin.chatprotocol.dto.DtoBase;

@Getter
public class EventBase extends DtoBase {
    private final String eventName;

    @JsonCreator
    public EventBase(@JsonProperty("eventName") String eventName) {
        this.eventName = eventName;
    }
}
