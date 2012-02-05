<?php

/**
 * @file
 * Provides integration with Views module.
 */

class RegistrationViewsController extends EntityDefaultViewsController {
  public function views_data() {
    $data = parent::views_data();

    return $data;
  }
}

/**
 * Implement hook_views_data_alter().
 * 
 * Create relationships between registrations, and their related entities.
 */
function registration_views_data_alter(&$data) {
  $registration_fields = field_read_fields(array('type' => 'registration'));
  $registration_instances = field_read_instances(array(
    'field_name' => array_keys($registration_fields)
  ));
  $entity_info = entity_get_info();

  foreach ($registration_instances as $instance) {
    $entity_type = $instance['entity_type'];
    $info = $entity_info[$entity_type];

    // entity to Registration.
    $data[$info['base table']]['registration_rel'] = array(
      'group' => t('Registration'),
      'title' => t('@entity to Registration',
        array('@entity' => ucfirst($info['label']))),
      'help' => t('The Registration associated with the @entity entity.',
        array('@entity' => ucfirst($info['label']))),
      'relationship' => array(
        'entity' => $entity_type,
        'handler' => 'registration_handler_relationship',
        'label' => t('@entity being the Registration',
          array('@entity' => $entity_type)),
        'base' => 'registration',
        'base field' => 'entity_id',
        'relationship field' => $info['entity keys']['id'],
      ),
    );

    // Registration to entity.
    $data['registration']['registration_related_' . $entity_type] = array(
      'group' => t('Registration'),
      'title' => t('Registration to @entity',
        array('@entity' => ucfirst($info['label']))),
      'help' => t('The @entity entity that is associated with the Registration.',
        array('@entity' => $info['label'])),
      'relationship' => array(
        'handler' => 'views_handler_relationship',
        'label' => t('@entity from Registration',
          array('@entity' => $entity_type)),
        'base' => $info['base table'],
        'base field' => $info['entity keys']['id'],
        'relationship field' => 'entity_id',
      ),
    );
  }
}
