<?php /**
 * @file
 * Contains \Drupal\registration\Plugin\Field\FieldFormatter\RegistrationType.
 */

namespace Drupal\registration\Plugin\Field\FieldFormatter;

use Drupal\Core\Field\FormatterBase;

/**
 * @FieldFormatter(
 *  id = "registration_type",
 *  label = @Translation("Registration Type"),
 *  field_types = {"registration"}
 * )
 */
class RegistrationType extends FormatterBase {

  /**
   * @FIXME
   * Move all logic relating to the registration_type formatter into this
   * class. For more information, see:
   *
   * https://www.drupal.org/node/1805846
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21FormatterInterface.php/interface/FormatterInterface/8
   * https://api.drupal.org/api/drupal/core%21lib%21Drupal%21Core%21Field%21FormatterBase.php/class/FormatterBase/8
   */

}
