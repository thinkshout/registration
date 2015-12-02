<?php
/**
 * @file
 * Page callbacks for Entity Registration Checkin.
 */

/**
 * Page callback: Show a list of active registrations that can be checked in.
 */
function registration_checkin_list_page($entity_type, $entity) {
  // Create table structure, header.
  $header = array(
    array(
      'data' => t('Registrant'),
      'field' => 'registrant_id',
      'type' => 'property',
      'specifier' => 'registrant_id',
    ),
    array(
      'data' => t('Email'),
    ),
    array(
      'data' => t('State'),
    ),
    array(
      'data' => t('Actions'),
    ),
  );

  list($entity_id) = entity_extract_ids($entity_type, $entity);
  $label = entity_label($entity_type, $entity);

  // Fetch registrations.
  $query = new EntityFieldQuery();
  $result = $query
    ->entityCondition('entity_type', 'registration')
    ->propertyCondition('entity_id', $entity_id)
    ->propertyCondition('entity_type', $entity_type)
    ->pager(20)
    ->tableSort($header)
    ->execute();

  // Build rows if we have registrations for the event.
  if (!empty($result['registration'])) {
    $registrations = registration_load_multiple(array_keys($result['registration']));
    $rows = array();

    // Build registrant rows.
    foreach ($registrations as $registration) {
      // Result basics.
      $wrapper = entity_metadata_wrapper('registration', $registration);
      $registrant_type = $wrapper->registrant->type();
      $registrant = $wrapper->registrant->value();
      $state = $wrapper->state->value();

      // Theme fancy table data.
      $registrant_entry = theme('registration_registrant_link', array(
        'registrant_type' => $registrant_type,
        'registrant' => $registrant,
      ));

      // Build the row.
      $rows[] = array(
        $registrant_entry,
        l($wrapper->registrant_mail->value(), 'mailto:' . $wrapper->registrant_mail->value()),
        $state->label,
        t('Check-in Button'),
      );
    }

    // Generate markup for the registrant table.
    $table = array(
      'header' => $header,
      'rows' => $rows,
    );
    $markup = theme('table', $table) . theme('pager');
  }
  else {
    $markup = t('There are no registrants for %name', array('%name' => $label));
  }

  return $markup;
}
