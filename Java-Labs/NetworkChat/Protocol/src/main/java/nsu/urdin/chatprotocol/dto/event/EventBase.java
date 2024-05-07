package nsu.urdin.chatprotocol.dto.event;

import lombok.AllArgsConstructor;
import lombok.Getter;

import java.io.Serializable;

@Getter
@AllArgsConstructor
public class EventBase implements Serializable {
    private final String eventName;
}
