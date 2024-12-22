<template>
	<div class="dashboard-layout">
	  <SidebarMenu />
	  <div class="main-content">
		<slot />
	  </div>
	</div>
</template>
  
<script>
import SidebarMenu from '@/components/SidebarMenu.vue';
  
export default {
	name: "DashboardLayout",
	components: {
	  SidebarMenu,
	},
};
</script>
  
<style scoped>
.dashboard-layout {
display: flex;
height: 100vh;
}
.main-content {
flex: 1;
padding: 20px;
overflow-y: auto;
}
</style>