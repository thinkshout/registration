<?php
namespace Drupal\registration_views;

class RegistrationViewsController extends EntityDefaultViewsController {

  public function views_data() {
    $data = parent::views_data();

    $data['registration']['view_registration'] = array(
      'field' => array(
        'title' => t('View link'),
        'help' => t('Provide a link to view a registration.'),
        'handler' => 'registration_handler_field_registration_link',
      ),
    );

    $data['registration']['edit_registration'] = array(
      'field' => array(
        'title' => t('Edit link'),
        'help' => t('Provide a link to edit a registration.'),
        'handler' => 'registration_handler_field_registration_link_edit',
      ),
    );

    $data['registration']['delete_registration'] = array(
      'field' => array(
        'title' => t('Delete link'),
        'help' => t('Provide a link to delete a registration.'),
        'handler' => 'registration_handler_field_registration_link_delete',
      ),
    );

    $data['registration']['state']['relationship']['base field'] = 'name';

    foreach (registration_get_types() as $registration_type) {
      $info = \Drupal::entityManager()->getDefinition($registration_type->registrant_entity_type);
      $relationship_key = $registration_type->registrant_entity_type . '_' . $info['entity keys']['id'];

      $data['registration'][$relationship_key]['title'] = t('Registrant @entity', array('@entity' => $info['label']));
      $data['registration'][$relationship_key]['help'] = t('The entity registered for this event.');
      $data['registration'][$relationship_key]['relationship'] = array(
        'handler' => 'views_handler_relationship',
        'base' => $info['base table'],
        'base field' => $info['entity keys']['id'],
        'label' => t('Registrant @entity', array('@entity' => $info['label'])),
        'relationship field' => 'registrant_id',
      );
    }

    $data['registration']['type']['relationship'] = array(
      'handler' => 'views_handler_relationship',
      'base' => 'registration_type',
      'base field' => 'name',
      'label' => t('Registration type'),
    );

    return $data;
  }

}
