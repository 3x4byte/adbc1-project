<template>
	<div class="sidebar-menu">
	  <ul>
      <li><router-link to="/dashboard">
        <div class="icon-wrapper">
          <img src="/src/icons/dashboard_dark.svg" alt="dashboard">
        </div>
      </router-link></li>

      <li><router-link to="/compare">
        <div class="icon-wrapper">
          <img src="/src/icons/compare_dark.svg" alt="compare">
        </div>
      </router-link></li>

      <li><router-link to="/logout">
        <div class="icon-wrapper">
          <img src="/src/icons/logout_dark.svg" alt="logout">
        </div>
      </router-link></li>
	  </ul>
	</div>
</template>

<script>
export default {
	name: "SidebarMenu",
};
</script>
  
<style scoped>
@import '../assets/base.css';

.sidebar-menu {
  position: fixed;
  top: 0;
  left: 0;
  bottom: 0;
  width: 70px;
  border-right: 2px solid var(--color-background-mute);
  padding: 10px;
  height: 100vh;
  z-index: 999;
}
.sidebar-menu ul {
  list-style: none;
  padding: 0;
}
.sidebar-menu li {
  margin: 10px 0;
  border-color: white;
  background-color: var(--color-background-soft);
  border-radius: 10px;
}

.sidebar-menu li:last-child {
  position: absolute;
  bottom: 0px;
}

.icon-wrapper{
  display: flex;
  justify-content: center;
  align-items: center;
  height: 50px;
  width: 50px;
}

</style>