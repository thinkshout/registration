<?php 
/**
 * @file
 * Display maximum slots for an entity.
 */
class registration_handler_field_entity_capacity_used extends views_handler_field_entity {
  function render($values) {
    $entity = $this->get_value($values);
    list($entity_id) = entity_extract_ids($this->entity_type, $entity);
    return (int)registration_event_count($entity_id, $this->entity_type);
  }
}
