<?php

namespace Drupal\registration\Entity;

use Drupal\Core\Config\Entity\ConfigEntityBase;

/**
 * Defines the Registration state entity.
 *
 * @ConfigEntityType(
 *   id = "registration_state",
 *   label = @Translation("Registration state"),
 *   handlers = {
 *     "list_builder" = "Drupal\registration\RegistrationStateListBuilder",
 *     "form" = {
 *       "add" = "Drupal\registration\Form\RegistrationStateForm",
 *       "edit" = "Drupal\registration\Form\RegistrationStateForm",
 *       "delete" = "Drupal\registration\Form\RegistrationStateDeleteForm"
 *     },
 *     "route_provider" = {
 *       "html" = "Drupal\registration\RegistrationStateHtmlRouteProvider",
 *     },
 *   },
 *   config_prefix = "registration_state",
 *   admin_permission = "administer site configuration",
 *   entity_keys = {
 *     "id" = "id",
 *     "label" = "label",
 *     "uuid" = "uuid"
 *   },
 *   links = {
 *     "canonical" = "/admin/structure/registration_state/{registration_state}",
 *     "add-form" = "/admin/structure/registration_state/add",
 *     "edit-form" = "/admin/structure/registration_state/{registration_state}/edit",
 *     "delete-form" = "/admin/structure/registration_state/{registration_state}/delete",
 *     "collection" = "/admin/structure/registration_state"
 *   }
 * )
 */
class RegistrationState extends ConfigEntityBase implements RegistrationStateInterface {

  /**
   * The Registration state ID.
   *
   * @var string
   */
  protected $id;

  /**
   * The Registration state label.
   *
   * @var string
   */
  protected $label;

}
