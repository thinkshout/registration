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

    // Entity Metadata does not handle registration schema well.
    $data['registration']['user_uid']['title'] = t('User');
    $data['registration']['user_uid']['help'] = t('The user registered for this event.');
    $data['registration']['user_uid']['relationship'] = array(
      'handler' => 'views_handler_relationship',
      'base' => 'users',
      'base field' => 'uid',
      'label' => t('User'),
    );

    $data['registration']['type']['relationship'] = array(
      'handler' => 'views_handler_relationship',
      'base' => 'registration_type',
      'base field' => 'name',
      'label' => t('Registration type'),
    );

    return $data;
  }

}
