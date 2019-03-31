<?php
require_once 'DbAuth.php';

function GetProfiles()
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        if ($result = mysqli_query($link, 'SELECT * FROM TableProfiles ORDER BY `TableProfiles`.ProfileId')) {
            while( $row = mysqli_fetch_assoc($result) ){
                $Profiles[$row['ProfileId']]=$row['Description'];
            }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $Profiles;
}

function GetCurrentProfile()
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        if ($result = mysqli_query($link, 'SELECT `TableProfiles`.Description FROM TableProfiles
            WHERE `TableProfiles`.ProfileId=`GetCurrentProfile`()')) {
                if( $row = mysqli_fetch_assoc($result) ){
                    $CurrentProfile=$row['Description'];
                }
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
    return $CurrentProfile;
}

function AddProfile($Profile)
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        $query='INSERT INTO `TableProfiles` (`Description`) VALUES ("'.$Profile.'")';
        if ($result = mysqli_query($link, $query)) 
        {
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
}

function DeleteProfile($Profile)
{
    $link = mysqli_connect(DB_SERVER, DB_USER, DB_PASS, DATABASE);
    if($link)
    {
        $query='DELETE FROM `TableProfiles` WHERE `TableProfiles`.Description="'.$Profile.'"';
        if ($result = mysqli_query($link, $query)) 
        {
            mysqli_free_result($result);
        }
        mysqli_close($link);
    }
}
?>